// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/CAssetManager.h"
#include "Character/PA_CharacterDefinition.h"
#include "Engine/Engine.h"

UCAssetManager& UCAssetManager::Get()
{
	UCAssetManager* Singleton = Cast<UCAssetManager>(GEngine->AssetManager.Get());
	if (Singleton)
	{
		return *Singleton;
	}

	UE_LOG(LogLoad, Fatal, TEXT("Asset Manager Needs to be of the type CAssetManager"));
	return (*NewObject<UCAssetManager>());
}

void UCAssetManager::LoadCharacterDefinitions(const FStreamableDelegate& LoadFinishedCallback)
{
	LoadPrimaryAssetsWithType(UPA_CharacterDefinition::GetCharacterDefinitionAssetType(), TArray<FName>(), LoadFinishedCallback);
}

bool UCAssetManager::GetLoadedCharacterDefinitions(TArray<UPA_CharacterDefinition*>& LoadedCharacterDefinitions) const
{
	TArray<UObject*> LoadedObjects;
	const bool bLoaded = GetPrimaryAssetObjectList(UPA_CharacterDefinition::GetCharacterDefinitionAssetType(), LoadedObjects);
	if (bLoaded)
	{
		for (UObject* LoadedObject : LoadedObjects)
		{
			LoadedCharacterDefinitions.Add(Cast<UPA_CharacterDefinition>(LoadedObject));
		}
	}

	return bLoaded;
}

void UCAssetManager::LoadShopItems(const FStreamableDelegate& LoadFinishedCallback)
{
	LoadPrimaryAssetsWithType(UPA_ShopItem::GetShopItemAssetType(), TArray<FName>(), FStreamableDelegate::CreateUObject(this, &ThisClass::ShopItemLoadFinished, LoadFinishedCallback));
}

bool UCAssetManager::GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutItems) const
{
	TArray<UObject*> LoadedObjects;
	const bool bLoaded = GetPrimaryAssetObjectList(UPA_ShopItem::GetShopItemAssetType(), LoadedObjects);

	if (bLoaded)
	{
		for (UObject* ObjectLoaded : LoadedObjects)
		{
			OutItems.Add(Cast<UPA_ShopItem>(ObjectLoaded));
		}
	}

	return bLoaded;
}

const FItemCollection* UCAssetManager::GetCombinationForItem(const UPA_ShopItem* Item) const
{
	return CombinationMap.Find(Item);
}

const FItemCollection* UCAssetManager::GetIngredientForItem(const UPA_ShopItem* Item) const
{
	return IngredientMap.Find(Item);
}

void UCAssetManager::ShopItemLoadFinished(FStreamableDelegate Callback)
{
	Callback.ExecuteIfBound();
	BuildItemMaps();
}

void UCAssetManager::BuildItemMaps()
{
	TArray<const UPA_ShopItem*> LoadedItems;
	if (GetLoadedShopItems(LoadedItems))
	{
		for (const UPA_ShopItem* Item : LoadedItems)
		{
			if (Item->GetIngredients().IsEmpty())
			{
				continue;
			}

			TArray<const UPA_ShopItem*> Items;
			for (const TSoftObjectPtr<UPA_ShopItem>& Ingredient : Item->GetIngredients())
			{
				const UPA_ShopItem* IngredientItem = Ingredient.LoadSynchronous();
				Items.Add(IngredientItem);
				AddToCombinationMap(IngredientItem, Item);
			}

			IngredientMap.Add(Item, FItemCollection{Items});
		}
	}
}

void UCAssetManager::AddToCombinationMap(const UPA_ShopItem* Ingredient, const UPA_ShopItem* CombinationItem)
{
	FItemCollection* Combinations = CombinationMap.Find(Ingredient);
	if (Combinations)
	{
		if (!Combinations->Contains(CombinationItem))
		{
			Combinations->AddItem(CombinationItem);
		}
	}
	else
	{
		CombinationMap.Add(Ingredient, FItemCollection{TArray<const UPA_ShopItem*>{CombinationItem}});
	}
}
