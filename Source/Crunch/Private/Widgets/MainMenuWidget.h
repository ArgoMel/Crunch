// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "MainMenuWidget.generated.h"

class UWidgetSwitcher;
class UCGameInstance;
class UEditableText;
class UScrollBox;
class USessionEntryWidget;
class UWaitingWidget;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
protected:	
	virtual void NativeOnInitialized() override;
	
	/******************************/	
	/*           Main             */
	/******************************/	
private:
	void SwitchToMainWidget() const;
	
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* MainSwitcher;

	UPROPERTY()
	UCGameInstance* CGameInstance;
	
	UPROPERTY(meta=(BindWidget))
	UWidget* MainWidgetRoot;

	/******************************/	
	/*           Session          */
	/******************************/	
	UFUNCTION()
	void CreateSessionBtnClicked();

	UFUNCTION()
	void CancelSessionCreation();

	UFUNCTION()
	void NewSessionNameTextChanged(const FText& NewText);

	void JoinSessionFailed() const;

	void UpdateLobbyList(const TArray<FOnlineSessionSearchResult>& SearchResults);
	
	UFUNCTION()
	void JoinSessionBtnClicked();

	void SessionEntrySelected(const FString& SelectedEntryIdStr);
	
	UPROPERTY(meta=(BindWidget))
	UButton* CreateSessionBtn;

	UPROPERTY(meta=(BindWidget))
	UEditableText* NewSessionNameText;

	UPROPERTY(meta=(BindWidget))
	UScrollBox* SessionScrollBox;

	UPROPERTY(meta=(BindWidget))
	UButton* JoinSessionBtn;

	UPROPERTY(EditDefaultsOnly, Category = "Session")
	TSubclassOf<USessionEntryWidget> SessionEntryWidgetClass;

	FString CurrentSelectedSessionId = "";

	/******************************/	
	/*           Login             */
	/******************************/	
private:
	UPROPERTY(meta=(BindWidget))
	UWidget* LoginWidgetRoot;

	UPROPERTY(meta=(BindWidget))
	UButton* LoginBtn;

	UFUNCTION()
	void LoginBtnClicked();

	void LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMsg) const;

	/******************************/	
	/*           Waiting          */
	/******************************/	
private:
	FOnButtonClickedEvent& SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowCancel = false) const;
	
	UPROPERTY(meta=(BindWidget))
	UWaitingWidget* WaitingWidget;
};
