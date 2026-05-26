// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayEffectTypes.h"
#include "ProjectileActor.generated.h"

UCLASS()
class AProjectileActor : public AActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:	
	AProjectileActor();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	
public:	
	void ShootProjectile(
	float InSpeed,
	float InMaxDistance,
	const AActor* InTarget,
	FGenericTeamId InTeamId,
	FGameplayEffectSpecHandle InHitEffectHandle
);
	
private:
	void TravelMaxDistanceReached();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Cue")
	FGameplayTag HitGameplayCueTag;

	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	UPROPERTY(Replicated)
	FVector MoveDir;

	UPROPERTY(Replicated)
	float ProjectileSpeed;

	UPROPERTY()
	const AActor* Target;

	FGameplayEffectSpecHandle HitEffectSpecHandle;
	FTimerHandle ShootTimerHandle;
};
