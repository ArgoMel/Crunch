// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_GroundPick.generated.h"

UCLASS()
class ATargetActor_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
public:
	ATargetActor_GroundPick();
	virtual void Tick(float DeltaTime) override;
	
	virtual void ConfirmTargetingAndContinue() override;

public:
	void SetTargetAreaRadius(float NewRadius);
	FORCEINLINE void SetTargetTraceRange(float NewRange) { TargetTraceRange = NewRange; }

	void SetTargetOptions(bool bTargetFriendly, bool bTargetEnemy = true);
	FORCEINLINE void SetShouldDrawDebug(bool bDrawDebug) { bShouldDrawDebug = bDrawDebug; }

private:
	FVector GetTargetPoint() const;
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Visual")
	UDecalComponent* DecalComp;

	bool bShouldTargetEnemy = true;
	bool bShouldTargetFriendly = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;

	bool bShouldDrawDebug = false;
};
