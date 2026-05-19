// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "StatsGauge.generated.h"

class UImage;
class UTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UStatsGauge : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	
private:
	void SetValue(float NewVal) const;
	
	void AttributeChanged(const FOnAttributeChangeData& Data) const;
	
private:
	UPROPERTY(meta=(BindWidget))
	UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AttributeText;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, Category = "Visual")
	UTexture2D* IconTexture;

	FNumberFormattingOptions NumberFormattingOptions;
};

