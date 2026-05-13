// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GAP_Launched.generated.h"

UCLASS()
class UGAP_Launched : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGAP_Launched();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
public:
	static FGameplayTag GetLaunchedAbilityActivationTag();
};
