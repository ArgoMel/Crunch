// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CharacterEntryWidget.generated.h"

class UPA_CharacterDefinition;
class UImage;
class UTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UCharacterEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
public:
	FORCEINLINE const UPA_CharacterDefinition* GetCharacterDefinition() const { return CharacterDefinition; }
	void SetSelected(bool bIsSelected) const;
	
private:	
	UPROPERTY(meta=(BindWidget))
	UImage* CharacterIcon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CharacterNameText;

	UPROPERTY()
	const UPA_CharacterDefinition* CharacterDefinition;
};
