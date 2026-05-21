// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

class UPA_ShopItem;
class UTileView;
class UInventoryComponent;
class UShopItemWidget;
class UItemTreeWidget;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UShopWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	
private:
	void LoadShopItems();
	void ShopItemLoadFinished() const;
	void ShopItemWidgetGenerated(UUserWidget& NewWidget);
	
	void ShowItemCombination(const UShopItemWidget* ItemWidget) const;
	
private:
	UPROPERTY(meta=(BindWidget))
	UTileView* ShopItemList;

	UPROPERTY(meta=(BindWidget))
	UItemTreeWidget* CombinationTree;

	UPROPERTY()
	TMap<const UPA_ShopItem*, const UShopItemWidget*> ItemsMap;

	UPROPERTY()
	UInventoryComponent* OwnerInventoryComponent;
};
