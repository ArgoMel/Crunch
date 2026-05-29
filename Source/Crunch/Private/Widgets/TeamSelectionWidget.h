// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamSelectionWidget.generated.h"

class UTextBlock;
class UButton;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, uint8 /*SlotID*/);

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UTeamSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	
public:
	void SetSlotID(uint8 NewSlotID);
	void UpdateSlotInfo(const FString& PlayerNickName) const;

private:
	UFUNCTION()
	void SelectButtonClicked();
	
public:
	FOnSlotClicked OnSlotClicked;
private:	
	UPROPERTY(meta=(BindWidget))
	UButton* SelectButton;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* InfoText;

	uint8 SlotID;
};

