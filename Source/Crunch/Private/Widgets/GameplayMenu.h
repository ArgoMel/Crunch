// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameplayMenu.generated.h"

class UTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UGameplayMenu : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	
public:
	FOnButtonClickedEvent& GetResumeButtonClickedEventDelegate() const;
	void SetTitleText(const FString& NewTitle) const;
	
private:
	UFUNCTION()
	void BackToMainMenu();

	UFUNCTION()
	void QuitGame();
	
private:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* MenuTitle;

	UPROPERTY(meta=(BindWidget))
	UButton* ResumeBtn;

	UPROPERTY(meta=(BindWidget))
	UButton* MainMenuBtn;

	UPROPERTY(meta=(BindWidget))
	UButton* QuitGameBtn;
};
