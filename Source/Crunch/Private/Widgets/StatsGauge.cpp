// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/StatsGauge.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UStatsGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Icon)
	{
		Icon->SetBrushFromTexture(IconTexture);
	}
}

void UStatsGauge::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	NumberFormattingOptions.MaximumFractionalDigits = 0;
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());

	if (OwnerASC)
	{
		bool bFound;
		const float AttributeVal = OwnerASC->GetGameplayAttributeValue(Attribute, bFound);
		SetValue(AttributeVal);

		OwnerASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ThisClass::AttributeChanged);
	}
}

void UStatsGauge::SetValue(float NewVal) const
{
	AttributeText->SetText(FText::AsNumber(NewVal, &NumberFormattingOptions));
}

void UStatsGauge::AttributeChanged(const FOnAttributeChangeData& Data) const
{
	SetValue(Data.NewValue);
}
