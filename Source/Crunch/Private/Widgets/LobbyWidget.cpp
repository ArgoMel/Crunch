// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/LobbyWidget.h"
#include "Character/PA_CharacterDefinition.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Crunch/Crunch.h"
#include "Framework/CAssetManager.h"
#include "Framework/CGameState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Network/CNetStatics.h"
#include "Player/CPlayerState.h"
#include "Player/LobbyPlayerController.h"
#include "Widgets/AbilityListView.h"
#include "Widgets/CharacterDisplay.h"
#include "Widgets/CharacterEntryWidget.h"
#include "Widgets/TeamSelectionWidget.h"
#include "Widgets/PlayerTeamLayoutWidget.h"

void ULobbyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ClearAndPopulateTeamSelectionSlots();
	ConfigureGameState();
	LobbyPlayerController = GetOwningPlayer<ALobbyPlayerController>();
	if (LobbyPlayerController.IsValid())
	{
		LobbyPlayerController->OnSwitchToHeroSelection.BindUObject(this, &ThisClass::SwitchToHeroSelection);
	}
	StartHeroSelectionButton->SetIsEnabled(false);
	StartHeroSelectionButton->OnClicked.AddDynamic(this, &ThisClass::StartHeroSelectionButtonClicked);
	StartMatchButton->SetIsEnabled(false);
	StartMatchButton->OnClicked.AddDynamic(this, &ThisClass::StartMatchButtonClicked);

	UCAssetManager::Get().LoadCharacterDefinitions(FStreamableDelegate::CreateUObject(this, &ThisClass::CharacterDefinitionLoaded));
	if (CharacterSelectionTileView)
	{
		CharacterSelectionTileView->OnItemSelectionChanged().AddUObject(this, &ThisClass::CharacterSelected);
	}

	SpawnCharacterDisplay();
}

void ULobbyWidget::ClearAndPopulateTeamSelectionSlots()
{
	TeamSelectionSlotGridPanel->ClearChildren();

	for (int i = 0; i < Crunch::ConstValue::MaxPlayerCount * 2; ++i)
	{
		UTeamSelectionWidget* NewSelectionSlot = CreateWidget<UTeamSelectionWidget>(this, TeamSelectionWidgetClass);
		if (NewSelectionSlot)
		{
			NewSelectionSlot->SetSlotID(i);
			UUniformGridSlot* NewGridSlot = TeamSelectionSlotGridPanel->AddChildToUniformGrid(NewSelectionSlot);
			if (NewGridSlot)
			{
				const int Row = i % Crunch::ConstValue::MaxPlayerCount;
				const int Column = i < Crunch::ConstValue::MaxPlayerCount ? 0 : 1;

				NewGridSlot->SetRow(Row);
				NewGridSlot->SetColumn(Column);
			}

			NewSelectionSlot->OnSlotClicked.AddUObject(this, &ThisClass::SlotSelected);
			TeamSelectionSlots.Add(NewSelectionSlot);
		}
	}
}

void ULobbyWidget::SlotSelected(uint8 NewSlotID) const
{
	if (LobbyPlayerController.IsValid())
	{
		LobbyPlayerController->Server_RequestSlotSelectionChange(NewSlotID);
	}
}

void ULobbyWidget::ConfigureGameState()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	CGameState = World->GetGameState<ACGameState>();
	if (!CGameState.IsValid())
	{
		World->GetTimerManager().SetTimer(ConfigureGameStateTimerHandle, this, &ThisClass::ConfigureGameState, 1.f);
	}
	else
	{
		CGameState->OnPlayerSelectionUpdated.AddUObject(this, &ThisClass::UpdatePlayerSelectionDisplay);
		UpdatePlayerSelectionDisplay(CGameState->GetPlayerSelection());
	}
}

void ULobbyWidget::UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections)
{
	for (const UTeamSelectionWidget* SelectionSlot : TeamSelectionSlots)
	{
		SelectionSlot->UpdateSlotInfo("Empty");
	}

	for (UUserWidget* CharacterEntryAsWidget : CharacterSelectionTileView->GetDisplayedEntryWidgets())
	{
		if (UCharacterEntryWidget* CharacterEntryWidget = Cast<UCharacterEntryWidget>(CharacterEntryAsWidget))
		{
			CharacterEntryWidget->SetSelected(false);
		}
	}

	for (const FPlayerSelection& PlayerSelection : PlayerSelections)
	{
		if (!PlayerSelection.IsValid())
		{
			continue;
		}
		TeamSelectionSlots[PlayerSelection.GetPlayerSlot()]->UpdateSlotInfo(PlayerSelection.GetPlayerNickName());

		UCharacterEntryWidget* SelectedEntry = CharacterSelectionTileView->GetEntryWidgetFromItem<UCharacterEntryWidget>(PlayerSelection.GetCharacterDefinition());
		if (SelectedEntry)
		{
			SelectedEntry->SetSelected(true);
		}

		if (PlayerSelection.IsForPlayer(GetOwningPlayerState()))
		{
			UpdateCharacterDisplay(PlayerSelection);
		}
	}

	if (CGameState.IsValid())
	{
		StartHeroSelectionButton->SetIsEnabled(CGameState->CanStartHeroSelection());
		StartMatchButton->SetIsEnabled(CGameState->CanStartMatch());
	}
	
	if (PlayerTeamLayoutWidget)
	{
		PlayerTeamLayoutWidget->UpdatePlayerSelection(PlayerSelections);
	}
}

void ULobbyWidget::StartHeroSelectionButtonClicked()
{
	if (LobbyPlayerController.IsValid())
	{
		LobbyPlayerController->Server_StartHeroSelection();
	}
}

void ULobbyWidget::SwitchToHeroSelection()
{
	MainSwitcher->SetActiveWidget(HeroSelectionRoot);
}

void ULobbyWidget::CharacterDefinitionLoaded()
{
	TArray<UPA_CharacterDefinition*> loadedCharacterDefinitions;
	if (UCAssetManager::Get().GetLoadedCharacterDefinitions(loadedCharacterDefinitions))
	{
		CharacterSelectionTileView->SetListItems(loadedCharacterDefinitions);
	}
}

void ULobbyWidget::CharacterSelected(UObject* SelectedUObject)
{
	if (!CPlayerState.IsValid())
	{
		CPlayerState = GetOwningPlayerState<ACPlayerState>();
	}

	if (!CPlayerState.IsValid())
		return;

	if (const UPA_CharacterDefinition* characterDefinition = Cast<UPA_CharacterDefinition>(SelectedUObject))
	{
		CPlayerState->Server_SetSelectedCharacterDefinition(characterDefinition);
	}
}

void ULobbyWidget::SpawnCharacterDisplay()
{
	if (CharacterDisplay.IsValid())
		return;

	if (!CharacterDisplayClass)
		return;

	FTransform CharacterDisplayTransform = FTransform::Identity;
	const AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	if (PlayerStart)
	{
		CharacterDisplayTransform = PlayerStart->GetActorTransform();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CharacterDisplay = GetWorld()->SpawnActor<ACharacterDisplay>(CharacterDisplayClass, CharacterDisplayTransform, SpawnParams);
	GetOwningPlayer()->SetViewTarget(CharacterDisplay.Get());
}

void ULobbyWidget::UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection)
{
	if (!PlayerSelection.GetCharacterDefinition())
		return;

	CharacterDisplay->ConfigureWithCharacterDefination(PlayerSelection.GetCharacterDefinition());
	AbilityListView->ClearListItems();
	const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>* Abilities = PlayerSelection.GetCharacterDefinition()->GetAbilities();
	if (Abilities)
	{
		AbilityListView->ConfigureAbilities(*Abilities);
	}
}

void ULobbyWidget::StartMatchButtonClicked()
{
	if (LobbyPlayerController.IsValid())
	{
		LobbyPlayerController->Server_RequestStartMatch();
	}
}
