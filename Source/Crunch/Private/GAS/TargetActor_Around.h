// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GenericTeamAgentInterface.h"
#include "TargetActor_Around.generated.h"

class USphereComponent;

UCLASS()
class ATargetActor_Around : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	ATargetActor_Around();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	/** Retrieve team identifier in form of FGenericTeamId */
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	
public:
	void ConfigureDetection(float DetectionRadius, const FGenericTeamId& InTeamId, const FGameplayTag& InLocalGameplayCueTag);
	
private:
	UFUNCTION()
	void OnRep_TargetDetectionRadiusReplicated() const;
	
	UFUNCTION()
	void ActorInDetectionRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	UPROPERTY(VisibleDefaultsOnly, Category = "Comp")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Targeting")
	USphereComponent* DetectionSphere;

	UPROPERTY(ReplicatedUsing = OnRep_TargetDetectionRadiusReplicated)
	float TargetDetectionRadius;

	UPROPERTY(Replicated)
	FGameplayTag LocalGameplayCueTag;
};

