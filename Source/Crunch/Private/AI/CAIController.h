// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "GameplayTagContainer.h"
#include "CAIController.generated.h"

class UAISenseConfig_Sight;

UCLASS()
class ACAIController : public AAIController
{
	GENERATED_BODY()
public:
	ACAIController();
	
protected:
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void BeginPlay() override;
	
private:
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus);
	UFUNCTION()
	void TargetForgotten(AActor* ForgottenActor);

	const UObject* GetCurrentTarget() const;
	void SetCurrentTarget(AActor* NewTarget);

	AActor* GetNextPerceivedActor() const;

	void ForgetActorIfDead(AActor* ActorToForget) const;

	void ClearAndDisableAllSenses();
	void EnableAllSenses() const;

	void PawnDeadTagUpdated(const FGameplayTag Tag, int32 Count);
	void PawnStunTagUpdated(const FGameplayTag Tag, int32 Count) const;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	UAIPerceptionComponent* AIPerceptionComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	UAISenseConfig_Sight* SightConfig;

	bool bIsPawnDead = false;
};
