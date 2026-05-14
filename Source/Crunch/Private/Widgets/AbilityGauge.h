// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayEffectTypes.h"
#include "AbilityGauge.generated.h"

class UTextBlock;
class UImage;
class UAbilitySystemComponent;
struct FGameplayAbilitySpec;
class UAbilityToolTip;

USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
public:
	void ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData);
	
private:
	void CreateToolTipWidget(const FAbilityWidgetData* AbilityWidgetData);
	
	void AbilityCommitted(UGameplayAbility* Ability);

	void StartCooldown(float CooldownTimeRemaining, float CooldownDuration);
	
	void CooldownFinished();
	void UpdateCooldown();
	
	void AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec);
	void UpdateCanCast();
	void UpgradePointUpdated(const FOnAttributeChangeData& Data);
	void ManaUpdated(const FOnAttributeChangeData& Data);
	
	const FGameplayAbilitySpec* GetAbilitySpec();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CanCastAbilityParamName = "CanCast";
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName UpgradePointAvailableParamName = "UpgradeAvailable";

	UPROPERTY(EditDefaultsOnly, Category = "Tool Tip")
	TSubclassOf<UAbilityToolTip> AbilityToolTipClass;

	UPROPERTY(meta=(BindWidget))
	UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	UImage* LevelGauge;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownCounterText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownDurationText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CostText;

	UPROPERTY()
	UGameplayAbility* AbilityCDO;

	float CachedCooldownDuration;
	float CachedCooldownTimeRemaining;

	FTimerHandle CooldownTimerHandle;
	FTimerHandle CooldownTimerUpdateHandle;

	FNumberFormattingOptions WholeNumberFormattingOptions;
	FNumberFormattingOptions TwoDigitNumberFormattingOptions;

	UPROPERTY()
	const UAbilitySystemComponent* OwnerAbilitySystemComponent;
	FGameplayAbilitySpecHandle CachedAbilitySpecHandle;

	bool bIsAbilityLearned = false;
};
