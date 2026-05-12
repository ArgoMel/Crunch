// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "MinionBarrack.generated.h"

class AMinion;

UCLASS()
class AMinionBarrack : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AMinionBarrack();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	const APlayerStart* GetNextSpawnSpot();

	void SpawnNewGroup();
	void SpawnNewMinions(int Amt);
	AMinion* GetNextAvailableMinion() const;

private:
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FGenericTeamId BarrackTeamId;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	int MinionPerGroup = 3;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float GroupSpawnInterval = 5.f;
	
	UPROPERTY()
	TArray<AMinion*> MinionPool;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	AActor* Goal;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AMinion> MinionClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<APlayerStart*> SpawnSpots;

	int NextSpawnSpotIndex = -1;

	FTimerHandle SpawnIntervalTimerHandle;
};
