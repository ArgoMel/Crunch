// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CCharacter.h"
#include "Minion.generated.h"

UCLASS()
class AMinion : public ACCharacter
{
	GENERATED_BODY()
public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;
	
private:
	virtual void OnRep_TeamID() override;
	
public:
	bool IsActive() const;
	void Activate() const;
	void SetGoal(AActor* Goal) const;

private:
	void PickSkinBasedOnTeamID();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TMap<FGenericTeamId, USkeletalMesh*> SkinMap;
};
