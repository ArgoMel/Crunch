// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerInfoTypes.h"
#include "CGameState.generated.h"

class UPA_CharacterDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSelectionUpdated, const TArray<FPlayerSelection>& /*NewPlayerSelection*/);

UCLASS()
class ACGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > &OutLifetimeProps) const override;
	
public:
	void RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot);
	void SetCharacterSelected(const APlayerState* SelectingPlayer, const UPA_CharacterDefinition* SelectedDefinition);
	bool IsSlotOccupied(uint8 SlotId) const;
	bool IsDefinitionSelected(const UPA_CharacterDefinition* Definition) const;
	void SetCharacterDeselected(const UPA_CharacterDefinition* DefinitionToDeselect);
	
	const TArray<FPlayerSelection>& GetPlayerSelection() const;
	bool CanStartHeroSelection() const;
	bool CanStartMatch() const;
	
private:	
	UFUNCTION()
	void OnRep_PlayerSelectionArray() const;
	
public:
	FOnPlayerSelectionUpdated OnPlayerSelectionUpdated;
	
private:	
	UPROPERTY(ReplicatedUsing = OnRep_PlayerSelectionArray)
	TArray<FPlayerSelection> PlayerSelectionArray;
};
