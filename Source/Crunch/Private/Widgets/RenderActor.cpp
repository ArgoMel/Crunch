// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/RenderActor.h"
#include "Components/SceneComponent.h"
#include "Components/SceneCaptureComponent2D.h"

ARenderActor::ARenderActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture Component");
	CaptureComponent->SetupAttachment(RootComp);
	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->FOVAngle = 30.f;
}

// Called when the game starts or when spawned
void ARenderActor::BeginPlay()
{
	Super::BeginPlay();
	CaptureComponent->ShowOnlyActorComponents(this);
	SetActorLocation(FVector{0.f, 0.f, 100000.f});
}

void ARenderActor::SetRenderTarget(UTextureRenderTarget2D* RenderTarget) const
{
	CaptureComponent->TextureTarget = RenderTarget;
}

void ARenderActor::UpdateRender() const
{
	if (CaptureComponent)
	{
		CaptureComponent->CaptureScene();
	}
}
