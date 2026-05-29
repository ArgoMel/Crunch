// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

UCLASS()
class AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
public:	
	virtual void OnRep_PlayerState() override;
protected:
	virtual void BeginPlay() override;
	
private:
	void SpawnWidget();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TSubclassOf<UUserWidget> MenuWidgetClass;

	UPROPERTY()
	UUserWidget* MenuWidget;
};
