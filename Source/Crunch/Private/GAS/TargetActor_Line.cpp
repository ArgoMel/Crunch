// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/TargetActor_Line.h"
#include "Crunch/Crunch.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbility.h"
#include "Kismet/KismetMathLibrary.h"

ATargetActor_Line::ATargetActor_Line()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;
	
	AvatarActor = nullptr;
	
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);

	TargetEndDetectionSphere = CreateDefaultSubobject<USphereComponent>("Target End Detection Sphere");
	TargetEndDetectionSphere->SetupAttachment(GetRootComponent());

	LazerVFX = CreateDefaultSubobject<UNiagaraComponent>("Lazer VFX");
	LazerVFX->SetupAttachment(GetRootComponent());
}

void ATargetActor_Line::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, TeamId);
	DOREPLIFETIME(ThisClass, TargetRange);
	DOREPLIFETIME(ThisClass, DetectionCylinderRadius);
	DOREPLIFETIME(ThisClass, AvatarActor);
}

void ATargetActor_Line::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	if (!OwningAbility)
	{
		return;
	}
	AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(PeriodicalTargetingTimerHandle, this, &ThisClass::DoTargetCheckAndReport, TargetingInterval, true);
	}
}

void ATargetActor_Line::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateTargetTrace();
}

void ATargetActor_Line::BeginDestroy()
{
	if (GetWorld() && PeriodicalTargetingTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(PeriodicalTargetingTimerHandle);
	}

	Super::BeginDestroy();
}

void ATargetActor_Line::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

void ATargetActor_Line::ConfigureTargetSetting(float NewTargetRange, float NewDetectionCylinderRadius, float NewTargetingInterval, FGenericTeamId OwnerTeamId, bool bShouldDrawDebug)
{
	TargetRange = NewTargetRange;
	DetectionCylinderRadius = NewDetectionCylinderRadius;
	TargetingInterval = NewTargetingInterval;
	SetGenericTeamId(OwnerTeamId);
	bDrawDebug = bShouldDrawDebug;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATargetActor_Line::DoTargetCheckAndReport()
{
	if (!HasAuthority())
	{
		return;
	}
	TSet<AActor*> OverlappingActorSet;
	TargetEndDetectionSphere->GetOverlappingActors(OverlappingActorSet);

	TArray<TWeakObjectPtr<AActor>> OverlappingActors;
	for (AActor* OverlappingActor : OverlappingActorSet)
	{
		if (ShouldReportActorAsTarget(OverlappingActor))
		{
			OverlappingActors.Add(OverlappingActor);
		}
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;

	FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray;
	ActorArray->SetActors(OverlappingActors);
	TargetDataHandle.Add(ActorArray);

	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}

void ATargetActor_Line::UpdateTargetTrace()
{
	FVector ViewLocation = GetActorLocation();
	FRotator ViewRotation = GetActorRotation();
	if (AvatarActor)
	{
		AvatarActor->GetActorEyesViewPoint(ViewLocation, ViewRotation);
	}

	FVector LookEndPoint = ViewLocation + ViewRotation.Vector() * 100000;
	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookEndPoint);
	SetActorRotation(LookRotation);

	FVector SweepEndLocation = GetActorLocation() + LookRotation.Vector() * TargetRange;

	TArray<FHitResult> HitResults;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	QueryParams.AddIgnoredActor(this);

	FCollisionResponseParams CollisionResponseParams(ECR_Overlap);
	GetWorld()->SweepMultiByChannel(HitResults, GetActorLocation(), SweepEndLocation, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(DetectionCylinderRadius), QueryParams, CollisionResponseParams);

	FVector LineEndLocation = SweepEndLocation;
	float LineLength = TargetRange;

	for (FHitResult& HitResult : HitResults)
	{
		if (HitResult.GetActor())
		{
			if (GetTeamAttitudeTowards(*HitResult.GetActor()) != ETeamAttitude::Friendly)
			{
				LineEndLocation = HitResult.ImpactPoint;
				LineLength = FVector::Distance(GetActorLocation() , LineEndLocation);
				break;
			}
		}
	}

	TargetEndDetectionSphere->SetWorldLocation(LineEndLocation);
	if (LazerVFX)
	{
		LazerVFX->SetVariableFloat(Crunch::VFXParam::Length, LineLength/100.f);
	}
}

bool ATargetActor_Line::ShouldReportActorAsTarget(const AActor* ActorToCheck) const
{
	if (!ActorToCheck)
	{
		return false;
	}
	if (ActorToCheck == AvatarActor)
	{
		return false;
	}
	if (ActorToCheck == this)
	{
		return false;
	}
	if (GetTeamAttitudeTowards(*ActorToCheck) != ETeamAttitude::Hostile)
	{
		return false;
	}
	return true;
}
