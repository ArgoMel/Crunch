// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GenericTeamAgentInterface.h"
#include "TargetActor_Line.generated.h"

class UNiagaraComponent;
class USphereComponent;

UCLASS()
class ATargetActor_Line : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	ATargetActor_Line();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;
	
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	/** Retrieve team identifier in form of FGenericTeamId */
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	
public:
	void ConfigureTargetSetting(
	float NewTargetRange,
	float NewDetectionCylinderRadius,
	float NewTargetingInterval,
	FGenericTeamId OwnerTeamId,
	bool bShouldDrawDebug);
	
private:
	void DoTargetCheckAndReport();

	void UpdateTargetTrace();

	bool ShouldReportActorAsTarget(const AActor* ActorToCheck) const;
	
private:
	UPROPERTY(Replicated)
	float TargetRange;

	UPROPERTY(Replicated)
	float DetectionCylinderRadius;

	UPROPERTY()
	float TargetingInterval;

	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	UPROPERTY()
	bool bDrawDebug;

	UPROPERTY(Replicated)
	const AActor* AvatarActor;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UNiagaraComponent* LazerVFX;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USphereComponent* TargetEndDetectionSphere;

	FTimerHandle PeriodicalTargetingTimerHandle;
};

