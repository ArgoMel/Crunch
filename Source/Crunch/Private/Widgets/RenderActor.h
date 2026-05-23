// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RenderActor.generated.h"

UCLASS()
class ARenderActor : public AActor
{
	GENERATED_BODY()
public:	
	ARenderActor();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	void SetRenderTarget(UTextureRenderTarget2D* RenderTarget) const;
	void UpdateRender() const;
	FORCEINLINE USceneCaptureComponent2D* GetCaptureComponent() const { return CaptureComponent; }

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Render Actor")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Render Actor")
	USceneCaptureComponent2D* CaptureComponent;
};
