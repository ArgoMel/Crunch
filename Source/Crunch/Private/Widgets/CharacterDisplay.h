// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterDisplay.generated.h"

class UPA_CharacterDefinition;
class UCameraComponent;

UCLASS()
class ACharacterDisplay : public AActor
{
	GENERATED_BODY()
public:	
	ACharacterDisplay();
	
public:	
	void ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* CharacterDefinition) const;

private:	
	UPROPERTY(VisibleDefaultsOnly, Category = "Character Display")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Character Display")
	UCameraComponent* ViewCameraComponent;
};
