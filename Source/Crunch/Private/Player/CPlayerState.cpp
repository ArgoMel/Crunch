// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/CPlayerState.h"
#include "Character/PA_CharacterDefinition.h"
#include "Character/CCharacter.h"
#include "Crunch/Crunch.h"
#include "Net/UnrealNetwork.h"
#include "Network/CNetStatics.h"
#include "Framework/CGameState.h"
#include "Kismet/GameplayStatics.h"

ACPlayerState::ACPlayerState()
{
	bReplicates = true;
	SetNetUpdateFrequency(100.f);
}

void ACPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, PlayerSelection);
}

void ACPlayerState::BeginPlay()
{
	Super::BeginPlay();
	CGameState = Cast<ACGameState>(UGameplayStatics::GetGameState(this));

	if (CGameState)
	{
		CGameState->OnPlayerSelectionUpdated.AddUObject(this, &ThisClass::PlayerSelectionUpdated);
	}
}

void ACPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ACPlayerState* NewPlayerState = Cast < ACPlayerState>(PlayerState);
	if (NewPlayerState)
	{
		NewPlayerState->PlayerSelection = PlayerSelection;
	}
}

TSubclassOf<APawn> ACPlayerState::GetSelectedPawnClass() const
{
	if (PlayerSelection.GetCharacterDefinition())
	{
		return PlayerSelection.GetCharacterDefinition()->LoadCharacterClass();
	}

	return nullptr;
}

FGenericTeamId ACPlayerState::GetTeamIdBasedOnSlot() const
{
	return PlayerSelection.GetPlayerSlot() < Crunch::ConstValue::MaxPlayerCount ? FGenericTeamId{ 0 } : FGenericTeamId{ 1 };
}

void ACPlayerState::Server_SetSelectedCharacterDefinition_Implementation(const UPA_CharacterDefinition* NewDefinition)
{
	if (!CGameState)
		return;

	if (!NewDefinition)
		return;

	if (CGameState->IsDefinitionSelected(NewDefinition))
		return;

	if (PlayerSelection.GetCharacterDefinition())
	{
		CGameState->SetCharacterDeselected(PlayerSelection.GetCharacterDefinition());
	}

	PlayerSelection.SetCharacterDefinition(NewDefinition);
	CGameState->SetCharacterSelected(this, NewDefinition);
}

bool ACPlayerState::Server_SetSelectedCharacterDefinition_Validate(const UPA_CharacterDefinition* NewDefinition)
{
	return true;
}

void ACPlayerState::PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelections)
{
	for (const FPlayerSelection& NewPlayerSelection : NewPlayerSelections)
	{
		if (NewPlayerSelection.IsForPlayer(this))
		{
			PlayerSelection = NewPlayerSelection;
		}
	}
}
