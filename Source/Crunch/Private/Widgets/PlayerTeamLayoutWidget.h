// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerInfoTypes.h"
#include "PlayerTeamLayoutWidget.generated.h"

class UPlayerTeamSlotWidget;
class UHorizontalBox;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UPlayerTeamLayoutWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	
public:
	void UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelections);
	
private:	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float PlayerTeamWidgetSlotMargin = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSubclassOf<UPlayerTeamSlotWidget> PlayerTeamSlotWidgetClass;

	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* TeamOneLayoutBox;

	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* TeamTwoLayoutBox;

	UPROPERTY()
	TArray<UPlayerTeamSlotWidget*> TeamSlotWidgets;
};
