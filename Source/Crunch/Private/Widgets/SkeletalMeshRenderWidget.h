// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/RenderActorWidget.h"
#include "SkeletalMeshRenderWidget.generated.h"

class ASkeletalMeshRenderActor;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class USkeletalMeshRenderWidget : public URenderActorWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
private:
	virtual void SpawnRenderActor() override; 
	virtual ARenderActor* GetRenderActor() const override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "SKeletal Mesh Render")
	TSubclassOf<ASkeletalMeshRenderActor> SkeletalMeshRenderActorClass;

	UPROPERTY()
	TWeakObjectPtr<ASkeletalMeshRenderActor> SkeletalMeshRenderActor;
};
