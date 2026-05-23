// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RenderActorWidget.generated.h"

class ARenderActor;
class UImage;
class USizeBox;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class URenderActorWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

private:
	void ConfigureRenderActor();
	virtual void SpawnRenderActor() PURE_VIRTUAL(URenderActorWidget::SpawnRenderActor, ); 
	virtual ARenderActor* GetRenderActor() const PURE_VIRTUAL(URenderActorWidget::GetRenderActor, return nullptr; );

	void BeginRenderCapture();
	void UpdateRender();
	void StopRenderCapture();

private:
	UPROPERTY(meta=(BindWidget))
	UImage* DisplayImage;

	UPROPERTY(meta=(BindWidget))
	USizeBox* RenderSizeBox;

	UPROPERTY(EditDefaultsOnly, Category = "Render Actor")
	FVector2D RenderSize;

	UPROPERTY(EditDefaultsOnly, Category = "Render Actor")
	int FrameRate = 24;

	float RenderTickInterval;
	FTimerHandle RenderTimerHandle;

	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget;
};
