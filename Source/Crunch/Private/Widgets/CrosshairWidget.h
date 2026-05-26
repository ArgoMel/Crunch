// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "CrosshairWidget.generated.h"

class UImage;
class UCanvasPanelSlot;

UCLASS(Abstract, BlueprintType)
class UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:
	void CrosshairTagUpdated(const FGameplayTag Tag, int32 NewCount) const;
	void UpdateCrosshairPosition() const;
	void TargetUpdated(const struct FGameplayEventData* EventData);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "View")
	FLinearColor HasTargetColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	FLinearColor NoTargetColor = FLinearColor::White;

	UPROPERTY(meta=(BindWidget))
	UImage* CrosshairImage;

	UPROPERTY()
	UCanvasPanelSlot* CrosshairCanvasPanelSlot;

	UPROPERTY()
	APlayerController* CachedPlayerController;

	UPROPERTY()
	const AActor* AimTarget;
};
