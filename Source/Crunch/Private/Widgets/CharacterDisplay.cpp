// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/CharacterDisplay.h"
#include "Camera/CameraComponent.h"
#include "Character/PA_CharacterDefinition.h"
#include "Components/SkeletalMeshComponent.h"

ACharacterDisplay::ACharacterDisplay()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root Comp"));

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh Component");
	MeshComponent->SetupAttachment(GetRootComponent());
	
	ViewCameraComponent = CreateDefaultSubobject<UCameraComponent>("View Camera Component");
	ViewCameraComponent->SetupAttachment(GetRootComponent());
	ViewCameraComponent->SetFieldOfView(30.f);
}

void ACharacterDisplay::ConfigureWithCharacterDefinition(const UPA_CharacterDefinition* CharacterDefinition) const
{
	if (!CharacterDefinition)
	{
		return;
	}
	MeshComponent->SetSkeletalMesh(CharacterDefinition->LoadDisplayMesh());
	MeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	MeshComponent->SetAnimInstanceClass(CharacterDefinition->LoadDisplayAnimationBP());
}

