// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityToolTip.generated.h"

class UTextBlock;
class UImage;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UAbilityToolTip : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetAbilityInfo(const FName& AbilityName, UTexture2D* AbilityTexture, const FText& AbilityDescription, float AbilityCooldown, float AbilityCost) const;

private:	
	UPROPERTY(meta=(BindWidget))	
	UTextBlock* AbilityNameText;

	UPROPERTY(meta=(BindWidget))	
	UImage* AbilityIcon;

	UPROPERTY(meta=(BindWidget))	
	UTextBlock* AbilityDescriptionText;

	UPROPERTY(meta=(BindWidget))	
	UTextBlock* AbilityCooldownText;

	UPROPERTY(meta=(BindWidget))	
	UTextBlock* AbilityCostText;
};
