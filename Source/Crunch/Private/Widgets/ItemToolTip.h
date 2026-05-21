// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToolTip.generated.h"

class UPA_ShopItem;
class UImage;
class UTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UItemToolTip : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetItem(const UPA_ShopItem* Item) const;
	void SetPrice(float newPrice) const;
	
private:
	UPROPERTY(meta=(BindWidget))
	UImage* IconImage;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemTitleText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemDescriptionText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemPriceText;
};
