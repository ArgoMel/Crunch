// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UValueGauge;
class UAbilityListView;
class UStatsGauge;
class UShopWidget;
class UInventoryWidget;
class USkeletalMeshRenderWidget;
class UMatchStatWidget;
class UGameplayMenu;
class UWidgetSwitcher;
class UCanvasPanel;
class UCrosshairWidget;

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "GameplayWidget.generated.h"

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	
public:
	void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities) const;
	void ToggleShop();

	UFUNCTION()
	void ToggleGameplayMenu();

	void ShowGameplayMenu();
	void SetGameplayMenuTitle(const FString& NewTitle) const;
	
private:
	void PlayShopPopupAnimation(bool bPlayForward);
	void SetOwningPawnInputEnabled(bool bPawnInputEnabled) const;
	void SetShowMouseCursor(bool bShowMouseCursor) const;
	void SetFocusToGameAndUI() const;
	void SetFocusToGameOnly() const;

private:
	UPROPERTY(meta=(BindWidget))
	UValueGauge* HealthBar;

	UPROPERTY(meta=(BindWidget))
	UValueGauge* ManaBar;

	UPROPERTY(meta=(BindWidget))
	UAbilityListView* AbilityListView;

	UPROPERTY(meta=(BindWidget))
	UStatsGauge* AttackDamageGauge;

	UPROPERTY(meta=(BindWidget))
	UStatsGauge* ArmorGauge;

	UPROPERTY(meta=(BindWidget))
	UStatsGauge* MoveSpeedGauge;

	UPROPERTY(meta=(BindWidget))
	UStatsGauge* IntelligenceGauge;

	UPROPERTY(meta=(BindWidget))
	UStatsGauge* StrengthGauge;

	UPROPERTY(meta=(BindWidget))
	UShopWidget* ShopWidget;

	UPROPERTY(meta=(BindWidget))
	UInventoryWidget* InventoryWidget;

	UPROPERTY(meta=(BindWidget))
	USkeletalMeshRenderWidget* HeadshotWidget;

	UPROPERTY(meta=(BindWidget))
	UMatchStatWidget* MatchStatWidget;

	UPROPERTY(meta=(BindWidget))
	UGameplayMenu* GameplayMenu;

	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* MainSwitcher;

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* GameplayWidgetRootPanel;

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* GameplayMenuRootPanel;

	UPROPERTY(meta=(BindWidget))
	UCrosshairWidget* CrosshairWidget;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ShopPopupAnimation;

	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
};
