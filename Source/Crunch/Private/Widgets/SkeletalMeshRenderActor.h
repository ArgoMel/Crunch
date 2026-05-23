// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/RenderActor.h"
#include "SkeletalMeshRenderActor.generated.h"

UCLASS()
class ASkeletalMeshRenderActor : public ARenderActor
{
	GENERATED_BODY()
public:
	ASkeletalMeshRenderActor();
protected:
	virtual void BeginPlay() override;
	
public:
	void ConfigureSkeletalMesh(USkeletalMesh* MeshAsset, TSubclassOf<UAnimInstance> AnimBlueprint) const;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Skeletal Mesh Renderer")
	USkeletalMeshComponent* MeshComp;
};
