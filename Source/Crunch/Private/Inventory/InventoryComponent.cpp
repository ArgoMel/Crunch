// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/InventoryComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Framework/CAssetManager.h"
#include "GAS/CHeroAttributeSet.h"
#include "Inventory/PA_ShopItem.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (OwnerAbilitySystemComponent.IsValid())
	{
		OwnerAbilitySystemComponent->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::AbilityCommitted);
	}
}

void UInventoryComponent::TryActivateItem(const FInventoryItemHandle& ItemHandle)
{
	const UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem)
	{
		return;
	}
	Server_ActivateItem(ItemHandle);
}

void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
	if (!OwnerAbilitySystemComponent.IsValid())
	{
		return;
	}
	Server_Purchase(ItemToPurchase);
}

void UInventoryComponent::SellItem(const FInventoryItemHandle& ItemHandle)
{
	Server_SellItem(ItemHandle);
}

float UInventoryComponent::GetGold() const
{
	bool bFound = false;
	if (OwnerAbilitySystemComponent.IsValid())
	{
		const float Gold = OwnerAbilitySystemComponent->GetGameplayAttributeValue(UCHeroAttributeSet::GetGoldAttribute(), bFound);
		if (bFound)
		{
			return Gold;
		}
	}

	return 0.f;
}

void UInventoryComponent::ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber) const
{
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		FoundItem->SetSlot(NewSlotNumber);
	}
}

UInventoryItem* UInventoryComponent::GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const
{
	UInventoryItem* const* FoundItem = InventoryMap.Find(Handle);
	if (FoundItem)
	{
		return *FoundItem;
	}

	return nullptr;
}

bool UInventoryComponent::IsFullFor(const UPA_ShopItem* Item) const
{
	if (!Item)
	{
		return false;
	}
	if (IsAllSlotOccupied())
	{
		return GetAvailableStackForItem(Item) == nullptr;
	}

	return false;
}

bool UInventoryComponent::IsAllSlotOccupied() const
{
	return InventoryMap.Num() >= GetCapacity();
}

UInventoryItem* UInventoryComponent::GetAvailableStackForItem(const UPA_ShopItem* Item) const
{
	if (!Item->GetIsStackable())
	{
		return nullptr;
	}
	for (const TPair<FInventoryItemHandle, UInventoryItem*>& ItemPair : InventoryMap)
	{
		if (ItemPair.Value 
			&& ItemPair.Value->IsForItem(Item) 
			&& !ItemPair.Value->IsStackFull())
		{
			return ItemPair.Value;
		}
	}

	return nullptr;
}

bool UInventoryComponent::FindIngredientForItem(const UPA_ShopItem* Item, TArray<UInventoryItem*>& OutIngredients, TArray<const UPA_ShopItem*>& IngredientToIgnore) const
{
	const FItemCollection* Ingredients = UCAssetManager::Get().GetIngredientForItem(Item);
	if (!Ingredients)
	{
		return false;
	}
	bool bAllFound = true;
	for (const UPA_ShopItem* Ingredient : Ingredients->GetItems())
	{
		if (IngredientToIgnore.Contains(Ingredient))
		{
			IngredientToIgnore.RemoveSingle(Ingredient);
			continue;
		}
		UInventoryItem* FoundItem = TryGetItemForShopItem(Ingredient);
		if (!FoundItem)
		{
			bAllFound = false;
			break;
		}

		OutIngredients.Add(FoundItem);
	}

	return bAllFound;
}

UInventoryItem* UInventoryComponent::TryGetItemForShopItem(const UPA_ShopItem* Item) const
{
	if (!Item)
	{
		return nullptr;
	}
	for (const TPair<FInventoryItemHandle, UInventoryItem*>& ItemHandlePair : InventoryMap)
	{
		if (ItemHandlePair.Value 
			&& ItemHandlePair.Value->GetShopItem() == Item)
		{
			return ItemHandlePair.Value;
		}
	}

	return nullptr;
}

void UInventoryComponent::TryActivateItemInSlot(int SlotNumber)
{
	for (const TPair<FInventoryItemHandle, UInventoryItem*>& ItemPair : InventoryMap)
	{
		if (ItemPair.Value->GetItemSlot() == SlotNumber)
		{
			Server_ActivateItem(ItemPair.Key);
			return;
		}
	}
}

void UInventoryComponent::AbilityCommitted(UGameplayAbility* CommittedAbility)
{
	if (!CommittedAbility)
	{
		return;
	}
	float CooldownTimeRemaining = 0.f;
	float CooldownDuration = 0.f;

	CommittedAbility->GetCooldownTimeRemainingAndDuration(
		CommittedAbility->GetCurrentAbilitySpecHandle(),
		CommittedAbility->GetCurrentActorInfo(),
		CooldownTimeRemaining,
		CooldownDuration
	);

	for (TPair<FInventoryItemHandle, UInventoryItem*>& ItemPair : InventoryMap)
	{
		if (!ItemPair.Value)
		{
			continue;
		}
		if (ItemPair.Value->IsGrantingAbility(CommittedAbility->GetClass()))
		{
			OnItemAbilityCommitted.Broadcast(ItemPair.Key, CooldownDuration, CooldownTimeRemaining);
		}
	}
}

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase)
	{
		return;
	}
	if (GetGold() < ItemToPurchase->GetPrice())
	{
		return;
	}
	if (!IsFullFor(ItemToPurchase))
	{
		OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
		GrantItem(ItemToPurchase);
		return;
	}
	
	if (TryItemCombination(ItemToPurchase))
	{
		OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
	}
}

bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	return true;
}

void UInventoryComponent::Server_ActivateItem_Implementation(FInventoryItemHandle ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem)
	{
		return;
	}
	InventoryItem->TryActivateGrantedAbility();
	const UPA_ShopItem* Item = InventoryItem->GetShopItem();
	if (Item->GetIsConsumable())
	{
		ConsumeItem(InventoryItem);
	}
}

bool UInventoryComponent::Server_ActivateItem_Validate(FInventoryItemHandle ItemHandle)
{
	return true;
}

void UInventoryComponent::Server_SellItem_Implementation(FInventoryItemHandle ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem || !InventoryItem->IsValid())
	{
		return;
	}
	const float SellPrice = InventoryItem->GetShopItem()->GetSellPrice();
	OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, SellPrice * InventoryItem->GetStackCount());
	RemoveItem(InventoryItem);
}

bool UInventoryComponent::Server_SellItem_Validate(FInventoryItemHandle ItemHandle)
{
	return true;
}

void UInventoryComponent::GrantItem(const UPA_ShopItem* NewItem)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (UInventoryItem* StackItem = GetAvailableStackForItem(NewItem))
	{
		StackItem->AddStackCount();
		OnItemStackCountChanged.Broadcast(StackItem->GetHandle(), StackItem->GetStackCount());
		Client_ItemStackCountChanged(StackItem->GetHandle(), StackItem->GetStackCount());
	}
	else
	{
		if (TryItemCombination(NewItem))
		{
			return;
		}
		UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
		const FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
		InventoryItem->InitItem(NewHandle, NewItem, OwnerAbilitySystemComponent.Get());
		InventoryMap.Add(NewHandle, InventoryItem);
		OnItemAdded.Broadcast(InventoryItem);
		//UE_LOG(LogTemp, Warning, TEXT("Server Adding Shop Item: %s, with Id: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleId());
		const FGameplayAbilitySpecHandle GrantedAbilitySpecHandle = InventoryItem->GetGrantedAbilitySpecHandle();
		Client_ItemAdded(NewHandle, NewItem, GrantedAbilitySpecHandle);
	}
}

void UInventoryComponent::ConsumeItem(UInventoryItem* Item)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (!Item)
	{
		return;
	}
	Item->ApplyConsumeEffect();
	if (!Item->ReduceStackCount())
	{
		RemoveItem(Item);
	}
	else
	{
		OnItemStackCountChanged.Broadcast(Item->GetHandle(), Item->GetStackCount());
		Client_ItemStackCountChanged(Item->GetHandle(), Item->GetStackCount());
	}
}

void UInventoryComponent::RemoveItem(UInventoryItem* Item)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	Item->RemoveGASModifications();
	OnItemRemoved.Broadcast(Item->GetHandle());
	InventoryMap.Remove(Item->GetHandle());
	Client_ItemRemoved(Item->GetHandle());
}

bool UInventoryComponent::TryItemCombination(const UPA_ShopItem* NewItem)
{
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}
	const FItemCollection* CombinationItems = UCAssetManager::Get().GetCombinationForItem(NewItem);
	if (!CombinationItems)
	{
		return false;
	}
	for (const UPA_ShopItem* CombinationItem : CombinationItems->GetItems())
	{
		TArray<UInventoryItem*> Ingredients;
		TArray<const UPA_ShopItem*> ignoreItems{NewItem};
		if (!FindIngredientForItem(CombinationItem, Ingredients, ignoreItems))
		{
			continue;
		}
		for (UInventoryItem* Ingredient : Ingredients)
		{
			RemoveItem(Ingredient);
		}

		GrantItem(CombinationItem);
		return true;
	}
	return false;
}

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* Item,  FGameplayAbilitySpecHandle GrantedAbilitySpecHandle)
{
	if (GetOwner()->HasAuthority())
	{
		return;
	}
	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	InventoryItem->InitItem(AssignedHandle, Item, OwnerAbilitySystemComponent.Get());
	InventoryItem->SetGrantedAbilitySpecHandle(GrantedAbilitySpecHandle);
	InventoryMap.Add(AssignedHandle, InventoryItem);
	OnItemAdded.Broadcast(InventoryItem);
	//UE_LOG(LogTemp, Warning, TEXT("Client Adding Shop Item: %s, with Id: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), AssignedHandle.GetHandleId());
}

void UInventoryComponent::Client_ItemRemoved_Implementation(FInventoryItemHandle ItemHandle)
{
	if (GetOwner()->HasAuthority())
	{
		return;
	}
	const UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem)
	{
		return;
	}
	InventoryItem->RemoveGASModifications();

	OnItemRemoved.Broadcast(ItemHandle);
	InventoryMap.Remove(ItemHandle);
}

void UInventoryComponent::Client_ItemStackCountChanged_Implementation(FInventoryItemHandle Handle, int NewCount)
{
	if (GetOwner()->HasAuthority())
	{
		return;
	}
	UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle);
	if (FoundItem)
	{
		FoundItem->SetStackCount(NewCount);
		OnItemStackCountChanged.Broadcast(Handle, NewCount);
	}
}