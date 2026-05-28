// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/TA_Blackhole.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Crunch/Crunch.h"

ATA_Blackhole::ATA_Blackhole()
{
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;
	PrimaryActorTick.bCanEverTick = true;
	
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);

	DetectionSphereComponent = CreateDefaultSubobject<USphereComponent>("Detection Sphere Component");
	DetectionSphereComponent->SetupAttachment(GetRootComponent());
	DetectionSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::ActorInBlackholeRange);
	DetectionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::ActorLeftBlackholeRange);
	
	VFXComponent = CreateDefaultSubobject<UParticleSystemComponent>("VFX Component");
	VFXComponent->SetupAttachment(GetRootComponent());
}

void ATA_Blackhole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, TeamId);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BlackholeRange, COND_None, REPNOTIFY_Always);
}

void ATA_Blackhole::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	const UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(BlackholeDurationTimerHandle, this, &ThisClass::StopBlackhole, BlackholeDuration);
	}
}

void ATA_Blackhole::Tick(float DeltaTime)
{
	if (HasAuthority())
	{
		for (const TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
		{
			AActor* Target = TargetPair.Key;
			UNiagaraComponent* NiagaraComponent = TargetPair.Value;

			FVector PullDir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
			Target->SetActorLocation(Target->GetActorLocation() + PullDir * PullSpeed * DeltaTime);

			if (NiagaraComponent)
			{
				NiagaraComponent->SetVariablePosition(Crunch::VFXParam::Origin, VFXComponent->GetComponentLocation());
			}
		}
	}
}

void ATA_Blackhole::ConfirmTargetingAndContinue()
{
	StopBlackhole();
}

void ATA_Blackhole::CancelTargeting()
{
	StopBlackhole();
	Super::CancelTargeting();
}

void ATA_Blackhole::SetGenericTeamId(const FGenericTeamId& InTeamId)
{
	TeamId = InTeamId;
}

void ATA_Blackhole::ConfigureBlackhole(float InBlackholeRange, float InPullSpeed, float InBlackholeDuration, const FGenericTeamId& InTeamId)
{
	PullSpeed = InPullSpeed;
	DetectionSphereComponent->SetSphereRadius(InBlackholeRange);
	SetGenericTeamId(InTeamId);
	BlackholeDuration = InBlackholeDuration;
	BlackholeRange = InBlackholeRange;
}

void ATA_Blackhole::OnRep_BlackholeRange() const
{
	DetectionSphereComponent->SetSphereRadius(BlackholeRange);
}

void ATA_Blackhole::ActorInBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TryAddTarget(OtherActor);
}

void ATA_Blackhole::ActorLeftBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	RemoveTarget(OtherActor);
}

void ATA_Blackhole::TryAddTarget(AActor* OtherTarget)
{
	if (!OtherTarget || ActorsInRangeMap.Contains(OtherTarget))
	{
		return;
	}
	if (GetTeamAttitudeTowards(*OtherTarget) != ETeamAttitude::Hostile)
	{
		return;
	}
	UNiagaraComponent* NiagaraComponent = nullptr;
	if (BlackholeLinkVFX)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(BlackholeLinkVFX, OtherTarget->GetRootComponent(), NAME_None, FVector::Zero(), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
		if (NiagaraComponent)
		{
			NiagaraComponent->SetVariablePosition(Crunch::VFXParam::Origin, VFXComponent->GetComponentLocation());
		}
	}

	ActorsInRangeMap.Add(OtherTarget, NiagaraComponent);
}

void ATA_Blackhole::RemoveTarget(AActor* OtherTarget)
{
	if (!OtherTarget)
	{
		return;
	}

	if (ActorsInRangeMap.Contains(OtherTarget))
	{
		UNiagaraComponent* VFXComp;
		ActorsInRangeMap.RemoveAndCopyValue(OtherTarget, VFXComp);
		if(IsValid(VFXComp))
		{
			VFXComp->DestroyComponent();
		}
	}
}

void ATA_Blackhole::StopBlackhole()
{
	TArray<TWeakObjectPtr<AActor>> FinalTargets;
	for (const TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
	{
		FinalTargets.Add(TargetPair.Key);
		UNiagaraComponent* NiagaraComponent = TargetPair.Value;
		if (IsValid(NiagaraComponent))
		{
			NiagaraComponent->DestroyComponent();
		}
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;

	FGameplayAbilityTargetData_ActorArray* TargetActorArray = new FGameplayAbilityTargetData_ActorArray;
	TargetActorArray->SetActors(FinalTargets);
	TargetDataHandle.Add(TargetActorArray);

	FGameplayAbilityTargetData_SingleTargetHit* BlowupLocation = new FGameplayAbilityTargetData_SingleTargetHit;
	BlowupLocation->HitResult.ImpactPoint = GetActorLocation();
	TargetDataHandle.Add(BlowupLocation);

	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}
