// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GenericTeamAgentInterface.h"
#include "TA_Blackhole.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UNiagaraComponent;

UCLASS()
class ATA_Blackhole : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	ATA_Blackhole();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void Tick(float DeltaTime) override;
	virtual void ConfirmTargetingAndContinue() override;
	virtual void CancelTargeting() override;
	
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamId) override;
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	
public:
	void ConfigureBlackhole(float InBlackholeRange, float InPullSpeed, float InBlackholeDuration, const FGenericTeamId& InTeamId);
	
private:
	UFUNCTION()
	void OnRep_BlackholeRange() const;
	
	UFUNCTION()
	void ActorInBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void ActorLeftBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TryAddTarget(AActor* OtherTarget);
	void RemoveTarget(AActor* OtherTarget);
	
	void StopBlackhole();
	
private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	float PullSpeed;
	float BlackholeDuration;
	FTimerHandle BlackholeDurationTimerHandle;

	UPROPERTY(ReplicatedUsing = OnRep_BlackholeRange)
	float BlackholeRange;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UNiagaraSystem* BlackholeLinkVFX;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USphereComponent* DetectionSphereComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UParticleSystemComponent* VFXComponent;

	UPROPERTY()
	TMap<AActor*, UNiagaraComponent*> ActorsInRangeMap;
};
