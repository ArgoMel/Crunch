// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerInfoTypes.h"
#include "LobbyWidget.generated.h"

class UWidgetSwitcher;
class UButton;
class UUniformGridPanel;
class UTeamSelectionWidget;
class UTileView;
class UAbilityListView;
class UPlayerTeamLayoutWidget;
class ALobbyPlayerController;
class ACPlayerState;
class ACGameState;
class ACharacterDisplay;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	
private:	
	void ClearAndPopulateTeamSelectionSlots();
	void SlotSelected(uint8 NewSlotID) const;
	
private:	
	void ConfigureGameState();
	
	void UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections);

	UFUNCTION()
	void StartHeroSelectionButtonClicked();

	void SwitchToHeroSelection();
	void CharacterDefinitionLoaded();

	void CharacterSelected(UObject* SelectedUObject);
	
	void SpawnCharacterDisplay();
	void UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection);

	UFUNCTION()
	void StartMatchButtonClicked();
	
private:	
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* MainSwitcher;

	UPROPERTY(meta=(BindWidget))	
	UWidget* TeamSelectionRoot;

	UPROPERTY(meta=(BindWidget))
	UButton* StartHeroSelectionButton;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* TeamSelectionSlotGridPanel;

	UPROPERTY(EditDefaultsOnly, Category = "TeamSelection")
	TSubclassOf<UTeamSelectionWidget> TeamSelectionWidgetClass;

	UPROPERTY()
	TArray<UTeamSelectionWidget*> TeamSelectionSlots;

	UPROPERTY(meta=(BindWidget))	
	UWidget* HeroSelectionRoot;

	UPROPERTY(meta=(BindWidget))	
	UTileView* CharacterSelectionTileView;

	UPROPERTY(meta=(BindWidget))	
	UAbilityListView* AbilityListView;

	UPROPERTY(meta=(BindWidget))	
	UPlayerTeamLayoutWidget* PlayerTeamLayoutWidget;

	UPROPERTY(meta=(BindWidget))	
	UButton* StartMatchButton;
	
	TWeakObjectPtr<ALobbyPlayerController> LobbyPlayerController;
	TWeakObjectPtr<ACPlayerState> CPlayerState;
	TWeakObjectPtr<ACGameState> CGameState;
	
	FTimerHandle ConfigureGameStateTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Character Display")
	TSubclassOf<ACharacterDisplay> CharacterDisplayClass;
	TWeakObjectPtr<ACharacterDisplay> CharacterDisplay;
};
