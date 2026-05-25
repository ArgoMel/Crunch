// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StormCore.generated.h"

class USphereComponent;
class UCameraComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGoalReachedDelegate, AActor* /*ViewTarget*/, int /*WiningTeam*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FonTeamInfluncerCountUpdatedDelegate, int /*TeamOneInfluencerCount*/, int /*TeamTwoInfluencerCount*/);

UCLASS()
class AStormCore : public ACharacter
{
	GENERATED_BODY()
public:
	AStormCore();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
public:	
	float GetProgress() const;
	
private:
	UFUNCTION()
	void NewInfluencerInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void InfluencerLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateTeamWeight();
	void UpdateGoal() const;
	
	UFUNCTION()
	void OnRep_CoreToCapture();

	void GoalReached(int WiningTeam);

	void CaptureCore();
	void ExpandFinished();
	 
public:
	FOnGoalReachedDelegate OnGoalReachedDelegate;
	FonTeamInfluncerCountUpdatedDelegate OnTeamInfluenceCountUpdated;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	UAnimMontage* ExpandMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	UAnimMontage* CaptureMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float InfluenceRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float MaxMoveSpeed = 500.f;

	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	USphereComponent* InfluenceRange;

	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	UDecalComponent* GroundDecalComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	UCameraComponent* ViewCam;

	UPROPERTY(EditAnywhere, Category = "Team")
	AActor* TeamOneGoal;

	UPROPERTY(EditAnywhere, Category = "Team")
	AActor* TeamTwoGoal;

	UPROPERTY(EditAnywhere, Category = "Team")
	AActor* TeamOneCore;

	UPROPERTY(EditAnywhere, Category = "Team")
	AActor* TeamTwoCore;

	UPROPERTY(ReplicatedUsing = OnRep_CoreToCapture)
	AActor* CoreToCapture;

	float CoreCaptureSpeed = 0.f;
	float TravelLength = 0.f;

	int TeamOneInfluencerCount = 0;
	int TeamTwoInfluencerCount = 0;

	float TeamWeight = 0.f;

	UPROPERTY()
	class AAIController* OwnerAIC;
};
