// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_Blink.generated.h"

class ATargetActor_GroundPick;

UCLASS()
class UGA_Blink : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Blink();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
public:
	static FGameplayTag GetTeleportationTag();
	
private:
	UFUNCTION()
	void GroundPickTargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void GroundPickCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void Teleport(FGameplayEventData Payload);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* TargetingMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* TeleportMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> GroundPickTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float BlinkLandTargetPushSpeed = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 1000;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlinkCastRange = 3000.f;

	FGameplayAbilityTargetDataHandle BlinkTargetDataHandle;
};
