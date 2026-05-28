// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GA_Blackhole.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor_GroundPick.h"
#include "GAS/TA_Blackhole.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/CAbilitySystemStatics.h"

void UGA_Blackhole::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	PlayCastBlackHoleMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	//PlayCastBlackHoleMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitPlacementTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	WaitPlacementTask->ValidData.AddDynamic(this, &ThisClass::PlaceBlackHole);
	WaitPlacementTask->Cancelled.AddDynamic(this, &ThisClass::PlacementCancelled);
	WaitPlacementTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitPlacementTask->BeginSpawningActor(this, TargetActorClass, TargetActor);
	ATargetActor_GroundPick* GroundPickTargetActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickTargetActor)
	{
		GroundPickTargetActor->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPickTargetActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickTargetActor->SetTargetTraceRange(TargetTraceRange);
	}
	WaitPlacementTask->FinishSpawningActor(this, TargetActor);
	AddAimEffect();
}

void UGA_Blackhole::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveAimEffect();
	RemoveFocusEffect();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Blackhole::PlaceBlackHole(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	RemoveAimEffect();
	AddFocusEffect();

	if (PlayCastBlackHoleMontageTask)
	{
		PlayCastBlackHoleMontageTask->OnBlendOut.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnCancelled.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnInterrupted.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnCompleted.RemoveAll(this);
	}

	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayHoldBlackholeMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HoldBlackHoleMontage);
		//PlayHoldBlackholeMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackholeMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackholeMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackholeMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackholeMontage->ReadyForActivation();
	}

	BlackHoleTargetingTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, BlackHoleTargetActorClass);
	BlackHoleTargetingTask->ValidData.AddDynamic(this, &ThisClass::FinalTargetsReceived);
	BlackHoleTargetingTask->Cancelled.AddDynamic(this, &ThisClass::FinalTargetsReceived);
	BlackHoleTargetingTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	BlackHoleTargetingTask->BeginSpawningActor(this, BlackHoleTargetActorClass, TargetActor);
	ATA_Blackhole* BlackholeTargetActor = Cast<ATA_Blackhole>(TargetActor);
	if (BlackholeTargetActor)
	{
		BlackholeTargetActor->ConfigureBlackhole(TargetAreaRadius, BlackHolePullSpeed,BlackHoleDuration, GetOwnerTeamId());
	}
	BlackHoleTargetingTask->FinishSpawningActor(this, TargetActor);
	if (BlackholeTargetActor)
	{
		BlackholeTargetActor->SetActorLocation(UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint);
	}
}

void UGA_Blackhole::PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	K2_EndAbility();
}

void UGA_Blackhole::FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(TargetDataHandle, FinalBlowDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		const FVector BlowCenter = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
		PushTargetsFromLocation(TargetDataHandle, BlowCenter,  BlowPushSpeed);

		UAbilityTask_PlayMontageAndWait* PlayFinalBlowMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FinalBlowMontage);
		//PlayFinalBlowMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayFinalBlowMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayFinalBlowMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayFinalBlowMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayFinalBlowMontage->ReadyForActivation();
	}
	else
	{
		PlayMontageLocally(FinalBlowMontage);
	}

	FGameplayCueParameters FinalBlowCueParams;
	FinalBlowCueParams.Location = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
	FinalBlowCueParams.RawMagnitude = TargetAreaRadius;

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(FinalBlowCueTag, FinalBlowCueParams);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), FinalBlowCueParams);
}

void UGA_Blackhole::AddAimEffect()
{
	AimEffectHandle = BP_ApplyGameplayEffectToOwner(AimEffect);
}

void UGA_Blackhole::RemoveAimEffect()
{
	if (AimEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(AimEffectHandle);
	}
}

void UGA_Blackhole::AddFocusEffect()
{
	FocusEffectHandle = BP_ApplyGameplayEffectToOwner(FocusEffect);
}

void UGA_Blackhole::RemoveFocusEffect()
{
	if (FocusEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(FocusEffectHandle);
	}
}
