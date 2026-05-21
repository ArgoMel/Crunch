// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

class UItemToolTip;
class UPA_ShopItem;
class UImage;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UItemWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual FReply NativeOnMouseButtonUp( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	
public:
	virtual void SetIcon(UTexture2D* IconTexture);
	
protected:
	UItemToolTip* SetToolTipWidget(const UPA_ShopItem* Item);
	UImage* GetItemIcon() const { return ItemIcon; }

private:
	virtual void RightButtonClicked();
	virtual void LeftButtonClicked();
	
private:
	UPROPERTY(meta=(BindWidget))
	UImage* ItemIcon;

	UPROPERTY(EditDefaultsOnly, Category = "ToolTip")
	TSubclassOf<UItemToolTip> ItemToolTipClass;
};
