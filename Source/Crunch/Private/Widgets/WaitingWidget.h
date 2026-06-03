// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "WaitingWidget.generated.h"

class UTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UWaitingWidget : public UUserWidget
{
	GENERATED_BODY()
public:	
	FOnButtonClickedEvent& ClearAndGetButtonClickedEvent() const;
	void SetWaitInfo(const FText& WaitInfo, bool bAllowCancel = false) const;

private:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WaitInfoText;

	UPROPERTY(meta=(BindWidget))
	UButton* CancelBtn;
};
