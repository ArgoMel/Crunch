// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/LevelGauge.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Crunch/Crunch.h"
#include "GAS/CHeroAttributeSet.h"

void ULevelGauge::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	NumberFormattingOptions.SetMaximumFractionalDigits(0);

	APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!OwnerPawn)
	{
		return;
	}
	UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn);
	if (!OwnerAbilitySystemComponent)
	{
		return;
	}
	OwnerASC = OwnerAbilitySystemComponent;
	
	UpdateGauge(FOnAttributeChangeData());
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetPrevLevelExperienceAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetLevelAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
}

void ULevelGauge::UpdateGauge(const FOnAttributeChangeData& Data) const
{
	bool bFound;
	const float CurrentExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetExperienceAttribute(), bFound);
	if (!bFound)
	{
		return;
	}
	const float NextLevelExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetNextLevelExperienceAttribute(), bFound);
	if (!bFound)
	{
		return;
	}
	const float PrevLevelExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetPrevLevelExperienceAttribute(), bFound);
	if (!bFound)
	{
		return;
	}
	const float CurrentLevel = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetLevelAttribute(), bFound);
	if (!bFound)
	{
		return;
	}

	LevelText->SetText(FText::AsNumber(CurrentLevel, &NumberFormattingOptions));

	const float Progress = CurrentExperience - PrevLevelExperience;
	const float LevelExpAmt = NextLevelExperience - PrevLevelExperience;

	float Percent = Progress / LevelExpAmt;

	if (NextLevelExperience == 0)
	{
		Percent = 1;
	}

	if (LevelProgressImage)
	{
		LevelProgressImage->GetDynamicMaterial()->SetScalarParameterValue(Crunch::MatParam::Percent, Percent);
	}
}
