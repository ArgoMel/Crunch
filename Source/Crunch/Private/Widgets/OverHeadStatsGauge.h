// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadStatsGauge.generated.h"

class UAbilitySystemComponent;
class UValueGauge;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UOverHeadStatsGauge : public UUserWidget
{
	GENERATED_BODY()
public:
	void ConfigureWithASC(UAbilitySystemComponent* AbilitySystemComponent) const;
	
private:
	UPROPERTY(meta=(BindWidget))
	UValueGauge* HealthBar;

	UPROPERTY(meta=(BindWidget))
	UValueGauge* ManaBar;
};
