// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_Freeze.generated.h"

class ATargetActor_GroundPick;

UCLASS()
class UGA_Freeze : public UCGameplayAbility
{
	GENERATED_BODY()
public:	
	UGA_Freeze();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void TargetingCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> TargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FreezeTargetingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FreezeCastMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag FreezingTargetGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag FreezingGameplayCueTag;
};

