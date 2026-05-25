// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchStatWidget.generated.h"

class UImage;
class UTextBlock;
class AStormCore;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UMatchStatWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	
private:
	void UpdateTeamInfluence(int TeamOneCount, int TeamTwoCount) const;

	void MatchFinished(AActor* ViewTarget, int WinningTeam);
	void UpdateProgress();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Match Stat")
	float ProgressUpdateInterval = 0.5f;

	UPROPERTY(meta=(BindWidget))
	UImage* ProgressImage;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TeamOneCountText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TeamTwoCountText;
	
	TWeakObjectPtr<AStormCore> StormCore;

	FTimerHandle UpdateProgressTimerHandle;
};
