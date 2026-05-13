// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/UpperCut.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/GA_Combo.h"
#include "GameplayTagsManager.h"

UUpperCut::UUpperCut()
{
	BlockAbilitiesWithTag.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag());
}

void UUpperCut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayUpperCutMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UpperCutMontage);
		//PlayUpperCutMontageTask->OnBlendOut.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnCancelled.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnInterrupted.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnCompleted.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitLaunchEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetUpperCutLaunchTag());
		WaitLaunchEventTask->EventReceived.AddDynamic(this, &ThisClass::StartLaunching);
		WaitLaunchEventTask->ReadyForActivation();
	}
	NextComboName = NAME_None;
}

FGameplayTag UUpperCut::GetUpperCutLaunchTag()
{
	return FGameplayTag::RequestGameplayTag("ability.uppercut.launch");
}

const FGenericDamageEffectDef* UUpperCut::GetDamageEffectDefForCurrentCombo() const
{
	const UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (OwnerAnimInstance)
	{
		const FName CurrentComboName = OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage);
		const FGenericDamageEffectDef* EffectDef = ComboDamageMap.Find(CurrentComboName);
		return EffectDef;
	}

	return nullptr;
}

void UUpperCut::StartLaunching(FGameplayEventData EventData)
{
	if (K2_HasAuthority())
	{
		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UpperCutLaunchSpeed);
		const int HitResultCount = UAbilitySystemBlueprintLibrary::GetDataCountFromTargetData(EventData.TargetData);

		for (int i = 0; i < HitResultCount; i++)
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(EventData.TargetData, i);
			PushTarget(HitResult.GetActor(), FVector::UpVector * UpperCutLaunchSpeed);
			ApplyGameplayEffectToHitResultActor(HitResult, LaunchDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		}
	}

	UAbilityTask_WaitGameplayEvent* WaitComboChangeEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UGA_Combo::GetComboChangedEventTag(), nullptr, false, false);
	WaitComboChangeEvent->EventReceived.AddDynamic(this, &ThisClass::HandleComboChangeEvent);
	WaitComboChangeEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitComboCommitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UCAbilitySystemStatics::GetBasicAttackInputPressedTag());
	WaitComboCommitEvent->EventReceived.AddDynamic(this, &ThisClass::HandleComboCommitEvent);
	WaitComboCommitEvent->ReadyForActivation();
	
	UAbilityTask_WaitGameplayEvent* WaitComboDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UGA_Combo::GetComboTargetEventTag());
	WaitComboDamageEvent->EventReceived.AddDynamic(this, &ThisClass::HandleComboDamageEvent);
	WaitComboDamageEvent->ReadyForActivation();
}

void UUpperCut::HandleComboChangeEvent(FGameplayEventData EventData)
{
	const FGameplayTag EventTag = EventData.EventTag;
	if (EventTag == UGA_Combo::GetComboChangedEventEndTag())
	{
		NextComboName = NAME_None;
		UE_LOG(LogTemp, Warning, TEXT("Next Combo is cleared"));
		return;
	}

	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);

	NextComboName = TagNames.Last();
	UE_LOG(LogTemp, Warning, TEXT("Next Combo is: %s"), *NextComboName.ToString());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UUpperCut::HandleComboCommitEvent(FGameplayEventData EventData)
{
	if (NextComboName == NAME_None)
	{
		return;
	}

	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (!OwnerAnimInst)
	{
		return;
	}

	OwnerAnimInst->Montage_SetNextSection(OwnerAnimInst->Montage_GetCurrentSection(UpperCutMontage), NextComboName, UpperCutMontage);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UUpperCut::HandleComboDamageEvent(FGameplayEventData EventData)
{
	if (K2_HasAuthority())
	{
		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UpperComboHoldSpeed);
		const FGenericDamageEffectDef* EffectDef = GetDamageEffectDefForCurrentCombo();
		if (!EffectDef)
		{
			return;
		}

		const int HitResultCount = UAbilitySystemBlueprintLibrary::GetDataCountFromTargetData(EventData.TargetData);
		for (int i = 0; i < HitResultCount; i++)
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(EventData.TargetData, i);
			FVector PushVel = GetAvatarActorFromActorInfo()->GetActorTransform().TransformVector(EffectDef->PushVelocity);
			PushTarget(HitResult.GetActor(), PushVel);
			ApplyGameplayEffectToHitResultActor(HitResult, EffectDef->DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		}
	}
}
