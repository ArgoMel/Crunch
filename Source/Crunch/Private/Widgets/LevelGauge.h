// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "LevelGauge.generated.h"

class UImage;
class UTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class ULevelGauge : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	
private:
	void UpdateGauge(const FOnAttributeChangeData& Data) const;
	
private:
	UPROPERTY(meta=(BindWidget))
	UImage* LevelProgressImage;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* LevelText;

	FNumberFormattingOptions NumberFormattingOptions;

	TWeakObjectPtr<UAbilitySystemComponent> OwnerASC;
};
