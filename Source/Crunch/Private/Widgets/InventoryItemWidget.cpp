// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InventoryItemWidget.h"
#include "Inventory/InventoryItem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Crunch/Crunch.h"
#include "Inventory/PA_ShopItem.h"
#include "Widgets/InventoryItemDragDropOp.h"
#include "Widgets/ItemToolTip.h"

void UInventoryItemWidget::SetIcon(UTexture2D* IconTexture)
{
	if (GetItemIcon())
	{
		GetItemIcon()->GetDynamicMaterial()->SetTextureParameterValue(Crunch::MatParam::Icon, IconTexture);
		return;
	}

	Super::SetIcon(IconTexture);
}

void UInventoryItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EmptySlot();
}

void UInventoryItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (!IsEmpty() && DragDropOpClass)
	{
		UInventoryItemDragDropOp* DragDropOp = NewObject<UInventoryItemDragDropOp>(this, DragDropOpClass);
		if (DragDropOp)
		{
			DragDropOp->SetDraggedItem(this);
			OutOperation = DragDropOp;
		}
	}
}

bool UInventoryItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UInventoryItemWidget* OtherWidget = Cast<UInventoryItemWidget>(InOperation->Payload))
	{
		if (OtherWidget && !OtherWidget->IsEmpty())
		{
			OnInventoryItemDropped.Broadcast(this, OtherWidget);
			return true;
		}
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryItemWidget::RightButtonClicked()
{
	if (!IsEmpty())
	{
		OnRightButtonClicked.Broadcast(GetItemHandle());
	}
}

void UInventoryItemWidget::LeftButtonClicked()
{
	if (!IsEmpty())
	{
		OnLeftButtonClicked.Broadcast(GetItemHandle());
	}
}

bool UInventoryItemWidget::IsEmpty() const
{
	return !InventoryItem || !InventoryItem->IsValid();
}

void UInventoryItemWidget::SetSlotNumber(int NewSlotNumber)
{
	SlotNumber = NewSlotNumber;
}

void UInventoryItemWidget::UpdateInventoryItem(const UInventoryItem* Item)
{
	UnBindCanCastAbilityDelegate();

	InventoryItem = Item;
	if (!InventoryItem 
		|| !InventoryItem->IsValid() 
		|| InventoryItem->GetStackCount() <= 0)
	{
		EmptySlot();
		return;
	}
	
	SetIcon(Item->GetShopItem()->GetIcon());
	const UItemToolTip* ToolTip = SetToolTipWidget(InventoryItem->GetShopItem());
	if (ToolTip)
	{
		ToolTip->SetPrice(InventoryItem->GetShopItem()->GetSellPrice());
	}

	if (InventoryItem->GetShopItem()->GetIsStackable())
	{
		StackCountText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UpdateStackCount();
	}
	else
	{
		StackCountText->SetVisibility(ESlateVisibility::Collapsed);
	}

	ClearCooldown();

	if (InventoryItem->IsGrantingAnyAbility())
	{
		UpdateCanCastDisplay(InventoryItem->CanCastAbility());
		const float AbilityCooldownRemaining = InventoryItem->GetAbilityCooldownTimeRemaining();
		const float AbilityCooldownDuration = InventoryItem->GetAbilityCooldownDuration();

		if (AbilityCooldownRemaining > 0.f)
		{
			StartCooldown(AbilityCooldownDuration, AbilityCooldownRemaining);
		}

		const float AbilityCost = InventoryItem->GetAbilityManaCost();
		ManaCostText->SetVisibility(AbilityCost == 0.f ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
		ManaCostText->SetText(FText::AsNumber(AbilityCost));

		CooldownDurationText->SetVisibility(AbilityCooldownDuration == 0.f? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
		CooldownDurationText->SetText(FText::AsNumber(AbilityCooldownDuration));
		BindCanCastAbilityDelegate();
	}
	else
	{
		UpdateCanCastDisplay(true);
		ManaCostText->SetVisibility(ESlateVisibility::Collapsed);
		CooldownDurationText->SetVisibility(ESlateVisibility::Collapsed);
		CooldownCountText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryItemWidget::EmptySlot()
{
	ClearCooldown();
	UnBindCanCastAbilityDelegate();
	InventoryItem = nullptr;
	SetIcon(EmptyTexture);
	SetToolTip(nullptr);

	StackCountText->SetVisibility(ESlateVisibility::Collapsed);
	ManaCostText->SetVisibility(ESlateVisibility::Collapsed);
	CooldownCountText->SetVisibility(ESlateVisibility::Collapsed);
	CooldownDurationText->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryItemWidget::UpdateStackCount() const
{
	if (InventoryItem)
	{
		StackCountText->SetText(FText::AsNumber(InventoryItem->GetStackCount()));
	}
}

UTexture2D* UInventoryItemWidget::GetIconTexture() const
{
	if (InventoryItem && InventoryItem->GetShopItem())
	{
		return InventoryItem->GetShopItem()->GetIcon();
	}

	return nullptr;
}

FInventoryItemHandle UInventoryItemWidget::GetItemHandle() const
{
	if (!IsEmpty())
	{
		return InventoryItem->GetHandle();
	}

	return FInventoryItemHandle::InvalidHandle();
}

void UInventoryItemWidget::StartCooldown(float CooldownDuration, float TimeRemaining)
{
	CooldownTimeRemaining = TimeRemaining;
	CooldownTimeDuration = CooldownDuration;
	GetWorld()->GetTimerManager().SetTimer(CooldownDurationTimerHandle, this, &ThisClass::CooldownFinished, CooldownTimeRemaining);
	GetWorld()->GetTimerManager().SetTimer(CooldownUpdateTimerHandle, this, &ThisClass::UpdateCooldown, CooldownUpdateInterval, true);

	CooldownCountText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UInventoryItemWidget::UpdateCanCastDisplay(bool bCanCast) const
{
	GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(Crunch::MatParam::CanCast, bCanCast ? 1.f : 0.f);
}

void UInventoryItemWidget::BindCanCastAbilityDelegate()
{
	if (InventoryItem)
	{
		const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdated.AddUObject(this, &ThisClass::UpdateCanCastDisplay);
	}
}

void UInventoryItemWidget::UnBindCanCastAbilityDelegate() const
{
	if (InventoryItem)
	{
		const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdated.RemoveAll(this);
	}
}

void UInventoryItemWidget::CooldownFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownUpdateTimerHandle);
	CooldownCountText->SetVisibility(ESlateVisibility::Collapsed);
	if (GetItemIcon())
	{
		GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(Crunch::MatParam::Percent, 1.f);
	}
}

void UInventoryItemWidget::UpdateCooldown()
{
	CooldownTimeRemaining -= CooldownUpdateInterval;
	const float CooldownAmt = 1.f - CooldownTimeRemaining / CooldownTimeDuration;
	CooldownDisplayFormattingOptions.MaximumFractionalDigits = CooldownTimeRemaining > 1.f ? 0 : 2;
	CooldownCountText->SetText(FText::AsNumber(CooldownTimeRemaining, &CooldownDisplayFormattingOptions));
	if (GetItemIcon())
	{
		GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(Crunch::MatParam::Percent, CooldownAmt);
	}
}

void UInventoryItemWidget::ClearCooldown()
{
	CooldownFinished();
}
