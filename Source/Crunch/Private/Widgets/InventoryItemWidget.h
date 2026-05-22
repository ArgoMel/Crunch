// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/ItemWidget.h"
#include "Inventory/InventoryItem.h"
#include "InventoryItemWidget.generated.h"

class UInventoryItem;
class UInventoryItemWidget;
class UTextBlock;
class UInventoryItemDragDropOp;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemDropped, UInventoryItemWidget* /*DestinationWidget*/, UInventoryItemWidget* /*SourceWidget*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnButtonClick, const FInventoryItemHandle& /*ItemHandle*/);

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()
public:
	virtual void SetIcon(UTexture2D* IconTexture) override;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnDragDetected( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation ) override;
	virtual bool NativeOnDrop( const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;
private:
	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked() override;
	
public:
	bool IsEmpty() const;
	void SetSlotNumber(int NewSlotNumber);
	void UpdateInventoryItem(const UInventoryItem* Item);
	void EmptySlot();
	FORCEINLINE int GetSlotNumber() const { return SlotNumber; }
	void UpdateStackCount() const;

	UTexture2D* GetIconTexture() const;
	FORCEINLINE const UInventoryItem* GetInventoryItem() const { return InventoryItem; }
	FInventoryItemHandle GetItemHandle() const;
	
	void StartCooldown(float CooldownDuration, float TimeRemaining);

private:
	void UpdateCanCastDisplay(bool bCanCast) const;
	
	void BindCanCastAbilityDelegate();
	void UnBindCanCastAbilityDelegate() const;

	void CooldownFinished();
	void UpdateCooldown();
	void ClearCooldown();
	
public:
	FOnInventoryItemDropped OnInventoryItemDropped;
	FOnButtonClick OnLeftButtonClicked;
	FOnButtonClick OnRightButtonClicked;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UTexture2D* EmptyTexture;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StackCountText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownCountText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownDurationText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ManaCostText;

	UPROPERTY()
	const UInventoryItem* InventoryItem;

	int SlotNumber;
	
	UPROPERTY(EditDefaultsOnly, Category = "Drag Drop")
	TSubclassOf<UInventoryItemDragDropOp> DragDropOpClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;
	
	FTimerHandle CooldownDurationTimerHandle;
	FTimerHandle CooldownUpdateTimerHandle;

	float CooldownTimeRemaining = 0.f;
	float CooldownTimeDuration = 0.f;
	
	FNumberFormattingOptions CooldownDisplayFormattingOptions;
};
