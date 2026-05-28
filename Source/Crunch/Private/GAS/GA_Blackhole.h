// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_Blackhole.generated.h"

class ATargetActor_GroundPick;
class ATA_Blackhole;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitTargetData;

UCLASS()
class UGA_Blackhole : public UCGameplayAbility
{
	GENERATED_BODY()
protected:	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UFUNCTION()
	void PlaceBlackHole(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	UFUNCTION()
	void PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	UFUNCTION()
	void FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	void AddAimEffect();
	void RemoveAimEffect();

	void AddFocusEffect();
	void RemoveFocusEffect();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlackHolePullSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlackHoleDuration = 6.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* TargetingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HoldBlackHoleMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FinalBlowMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<UGameplayEffect> AimEffect;

	FActiveGameplayEffectHandle AimEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<UGameplayEffect> FocusEffect;

	FActiveGameplayEffectHandle FocusEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> TargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATA_Blackhole> BlackHoleTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> FinalBlowDamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float BlowPushSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	FGameplayTag FinalBlowCueTag;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayCastBlackHoleMontageTask;

	UPROPERTY()
	UAbilityTask_WaitTargetData* BlackHoleTargetingTask;
};
