// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "ValueGauge.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UValueGauge : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativePreConstruct() override;
	
public:
	void SetAndBoundToGameplayAttribute(class UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute);
	void SetValue(float NewValue, float NewMaxValue);

private:
	void ValueChanged(const FOnAttributeChangeData& ChangedData);
	void MaxValueChanged(const FOnAttributeChangeData& ChangedData);
	
private:
	float CachedValue;
	float CachedMaxValue;

	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor BarColor;

	UPROPERTY(EditAnywhere, Category = "Visual")
	FSlateFontInfo ValueTextFont;

	UPROPERTY(EditAnywhere, Category = "Visual")
	bool bValueTextVisible = true;
	
	UPROPERTY(EditAnywhere, Category = "Visual")
	bool bProgressBarVisible = true;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	UTextBlock* ValueText;
};
