// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTeamSlotWidget.generated.h"

class UPA_CharacterDefinition;
class UImage;
class UTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UPlayerTeamSlotWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual void NativeOnMouseLeave( const FPointerEvent& InMouseEvent ) override;
	
public:
	void UpdateSlot(const FString& PlayerName, const UPA_CharacterDefinition* CharacterDefinition);

private:
	void UpdateNameText() const;
	
private:	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* HoverAnim;

	UPROPERTY(meta=(BindWidget))
	UImage* PlayerCharacterIcon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* NameText;

	FString CachedPlayerNameStr;
	FString CachedCharacterNameStr;
};
