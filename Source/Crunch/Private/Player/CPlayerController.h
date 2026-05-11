// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "CPlayerController.generated.h"

class ACPlayerCharacter;
class UGameplayWidget;
class UInputMappingContext;
class UInputAction;

UCLASS()
class ACPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	// only called on the client, also on the listening server.
	virtual void AcknowledgePossession(APawn* NewPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

protected:
	// only called on the server
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void SetupInputComponent() override;
	
public:
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;
	
public:
	void MatchFinished(AActor* ViewTarget, int WiningTeam);
	
private:
	UFUNCTION(Client, Reliable)
	void Client_MatchFinished(AActor* ViewTarget, int WiningTeam);

	void SpawnGameplayWidget();

	UFUNCTION()
	void ToggleShop();
	
	UFUNCTION()
	void ToggleGameplayMenu();
	void ShowWinLoseState();
	
private:
	UPROPERTY(EditDefaultsOnly, Category="View")
	float MatchFinishViewBlendTimeDuration = 2.f;

	UPROPERTY()
	ACPlayerCharacter* CPlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	UGameplayWidget* GameplayWidget;

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* UIInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ShopToggleInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ToggleGameplayMenuAction;
};
