// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryItem.h"
#include "InventoryWidget.generated.h"

class UInventoryItemWidget;
class UInventoryContextMenuWidget;
class UWrapBox;
class UInventoryComponent;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
	
private:
	void SpawnContextMenu();

	UFUNCTION()
	void SellFocusedItem();

	UFUNCTION()
	void UseFocusedItem();

	void SetContextMenuVisible(bool bContextMenuVisible) const;
	void ToggleContextMenu(const FInventoryItemHandle& ItemHandle);
	void ClearContextMenu();
	
	void ItemAdded(const UInventoryItem* InventoryItem);
	void ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount);

	UInventoryItemWidget* GetNextAvailableSlot() const;

	void HandleItemDragDrop(UInventoryItemWidget* DestinationWidget, UInventoryItemWidget* SourceWidget);
	void ItemRemoved(const FInventoryItemHandle& ItemHandle);
	void ItemAbilityCommitted(const FInventoryItemHandle& ItemHandle, float CooldownDuration, float CooldownTimeRemaining);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryContextMenuWidget> ContextMenuWidgetClass;

	UPROPERTY()
	UInventoryContextMenuWidget* ContextMenuWidget;
	
	FInventoryItemHandle CurrentFocusedItemHandle;

	UPROPERTY(meta=(BindWidget))
	UWrapBox* ItemList;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryItemWidget> ItemWidgetClass;
	
	TWeakObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY()
	TArray<UInventoryItemWidget*> ItemWidgets;
	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItemWidget*> PopulatedItemEntryWidgets;
};
