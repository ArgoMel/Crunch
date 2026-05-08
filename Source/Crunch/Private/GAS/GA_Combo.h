// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_Combo.generated.h"

UCLASS()
class UGA_Combo : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Combo();
	static FGameplayTag GetComboChangedEventTag();
	static FGameplayTag GetComboChangedEventEndTag();
	static FGameplayTag GetComboTargetEventTag();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	void SetupWaitComboInputPress();

	UFUNCTION()
	void HandleInputPress(float TimeWaited);

	void TryCommitCombo() const;
	TSubclassOf<UGameplayEffect> GetDamageEffectForCurrentCombo() const;

	UFUNCTION()
	void ComboChangedEventReceived(FGameplayEventData Data);

	UFUNCTION()
	void DoDamage(FGameplayEventData Data);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TMap<FName, TSubclassOf<UGameplayEffect>> DamageEffectMap;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ComboMontage;
	
	FName NextComboName;
};
