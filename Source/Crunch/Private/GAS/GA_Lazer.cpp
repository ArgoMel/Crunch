// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GA_Lazer.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Crunch/Crunch.h"
#include "GAS/CAttributeSet.h"
#include "GAS/TargetActor_Line.h"

void UGA_Lazer::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !LazerMontage)
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayerLazerMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, LazerMontage);
		//PlayerLazerMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerLazerMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerLazerMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerLazerMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayerLazerMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitShootEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetShootTag());
		WaitShootEvent->EventReceived.AddDynamic(this, &ThisClass::ShootLazer);
		WaitShootEvent->ReadyForActivation();

		UAbilityTask_WaitCancel* waitCancel = UAbilityTask_WaitCancel::WaitCancel(this);
		waitCancel->OnCancel.AddDynamic(this, &ThisClass::K2_EndAbility);
		waitCancel->ReadyForActivation();
	}
}

void UGA_Lazer::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* OwnerAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (OwnerAbilitySystemComponent && OnGoingConsumeEffectHandle.IsValid())
	{
		OwnerAbilitySystemComponent->RemoveActiveGameplayEffect(OnGoingConsumeEffectHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FGameplayTag UGA_Lazer::GetShootTag()
{
	return FGameplayTag::RequestGameplayTag("ability.lazer.shoot");
}

void UGA_Lazer::ShootLazer(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		OnGoingConsumeEffectHandle = BP_ApplyGameplayEffectToOwner(OnGoingConsumeEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		UAbilitySystemComponent* OwnerAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
		if (OwnerAbilitySystemComponent)
		{
			OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::ManaUpdated);
		}
	}

	UAbilityTask_WaitTargetData* WaitDamageTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, LazerTargetActorClass);
	WaitDamageTargetTask->ValidData.AddDynamic(this, &ThisClass::TargetReceived);
	WaitDamageTargetTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitDamageTargetTask->BeginSpawningActor(this, LazerTargetActorClass, TargetActor);
	ATargetActor_Line* LineTargetActor = Cast<ATargetActor_Line>(TargetActor);
	if (LineTargetActor)
	{
		LineTargetActor->ConfigureTargetSetting(TargetRange, DetectionCylinderRadius, TargetingInterval, GetOwnerTeamId(), ShouldDrawDebug());
	}

	WaitDamageTargetTask->FinishSpawningActor(this, TargetActor);

	if(LineTargetActor)
	{
		LineTargetActor->AttachToComponent(GetOwningComponentFromActorInfo(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Crunch::SocketName::Lazer);
	}
}

void UGA_Lazer::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	UAbilitySystemComponent* OwnerAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (OwnerAbilitySystemComponent 
		&& !OwnerAbilitySystemComponent->CanApplyAttributeModifiers(OnGoingConsumeEffect.GetDefaultObject(), GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo), MakeEffectContext(CurrentSpecHandle, CurrentActorInfo)))
	{
		K2_EndAbility();
	}
}

void UGA_Lazer::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(TargetDataHandle, HitDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	}
	PushTargets(TargetDataHandle, GetAvatarActorFromActorInfo()->GetActorForwardVector() * HitPushSpeed);
}
