// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/StormCore.h"
#include "AIController.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Components/CapsuleComponent.h"
#include "Crunch/Crunch.h"
#include "Net/UnrealNetwork.h"

AStormCore::AStormCore()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->CanCharacterStepUpOn=ECB_Yes;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target,ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	InfluenceRange = CreateDefaultSubobject<USphereComponent>("Influence Range");
	InfluenceRange->SetupAttachment(GetRootComponent());
	InfluenceRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	InfluenceRange->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	InfluenceRange->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::NewInfluencerInRange);
	InfluenceRange->OnComponentEndOverlap.AddDynamic(this, &ThisClass::InfluencerLeftRange);

	ViewCam = CreateDefaultSubobject<UCameraComponent>("View Cam");
	ViewCam->SetupAttachment(GetRootComponent());

	GroundDecalComponent = CreateDefaultSubobject<UDecalComponent>("Ground Decal Component");
	GroundDecalComponent->SetupAttachment(GetRootComponent());
}

void AStormCore::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CoreToCapture, COND_None, REPNOTIFY_Always);
}

void AStormCore::BeginPlay()
{
	Super::BeginPlay();
	const FVector TeamOneGoalLoc = TeamOneGoal->GetActorLocation();
	const FVector TeamTwoGoalLoc = TeamTwoGoal->GetActorLocation();

	FVector GoalOffset = TeamOneGoalLoc - TeamTwoGoalLoc;
	GoalOffset.Z = 0;

	TravelLength = GoalOffset.Length();
}

void AStormCore::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	OwnerAIC = Cast<AAIController>(NewController);
}

void AStormCore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CoreToCapture)
	{
		const FVector CoreMoveDir = (GetMesh()->GetComponentLocation() - CoreToCapture->GetActorLocation()).GetSafeNormal();
		CoreToCapture->AddActorWorldOffset(CoreMoveDir * CoreCaptureSpeed * DeltaTime);
	}
}

#if WITH_EDITOR
void AStormCore::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName PropertyName = PropertyChangedEvent.GetPropertyName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, InfluenceRadius))
	{
		InfluenceRange->SetSphereRadius(InfluenceRadius);
		const FVector DecalSize = GroundDecalComponent->DecalSize;
		GroundDecalComponent->DecalSize = FVector{DecalSize.X, InfluenceRadius, InfluenceRadius};
	}
}
#endif

float AStormCore::GetProgress() const
{
	const FVector TeamTwoGoalLoc = TeamTwoGoal->GetActorLocation();
	FVector VectorFromTeamOne = GetActorLocation() - TeamTwoGoalLoc;
	VectorFromTeamOne.Z = 0.f;

	return VectorFromTeamOne.Length() / TravelLength;
}

void AStormCore::NewInfluencerInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == TeamOneGoal)
	{
		GoalReached(0);
	}

	if (OtherActor == TeamTwoGoal)
	{
		GoalReached(1);
	}

	const IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (OtherTeamInterface)
	{
		if (OtherTeamInterface->GetGenericTeamId().GetId() == 0)
		{
			++TeamOneInfluencerCount;
		}
		else if (OtherTeamInterface->GetGenericTeamId().GetId() == 1)
		{
			++TeamTwoInfluencerCount;
		}
		UpdateTeamWeight();
	}
}

void AStormCore::InfluencerLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (OtherTeamInterface)
	{
		if (OtherTeamInterface->GetGenericTeamId().GetId() == 0)
		{
			--TeamOneInfluencerCount;
			TeamOneInfluencerCount=FMath::Max(TeamOneInfluencerCount, 0);
		}
		else if (OtherTeamInterface->GetGenericTeamId().GetId() == 1)
		{
			--TeamTwoInfluencerCount;
			TeamTwoInfluencerCount=FMath::Max(TeamTwoInfluencerCount, 0);
		}
		UpdateTeamWeight();
	}
}

void AStormCore::UpdateTeamWeight()
{
	OnTeamInfluenceCountUpdated.Broadcast(TeamOneInfluencerCount, TeamTwoInfluencerCount);
	if (TeamOneInfluencerCount == TeamTwoInfluencerCount)
	{
		TeamWeight = 0.f;
	}
	else
	{
		const float TeamOffset = TeamOneInfluencerCount - TeamTwoInfluencerCount;
		const float TeamTotal = TeamOneInfluencerCount + TeamTwoInfluencerCount;

		TeamWeight = TeamOffset / TeamTotal;
	}

	UpdateGoal();
}

void AStormCore::UpdateGoal() const
{
	if (!HasAuthority())
	{
		return;
	}
	if (!OwnerAIC)
	{
		return;
	}
	if (!GetCharacterMovement())
	{
		return;
	}
	if (TeamWeight > 0)
	{
		OwnerAIC->MoveToActor(TeamOneGoal);
	}
	else
	{
		OwnerAIC->MoveToActor(TeamTwoGoal);
	}

	const float Speed = MaxMoveSpeed * FMath::Abs(TeamWeight);

	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AStormCore::OnRep_CoreToCapture()
{
	if (CoreToCapture)
	{
		CaptureCore();
	}
}

void AStormCore::GoalReached(int WiningTeam)
{
	OnGoalReachedDelegate.Broadcast(this, WiningTeam);

	if (!HasAuthority())
	{
		return;
	}
	MaxMoveSpeed = 0.f;
	CoreToCapture = WiningTeam == 0 ? TeamTwoCore : TeamOneCore;
	CaptureCore();
}

void AStormCore::CaptureCore()
{
	const float ExpandDuration = GetMesh()->GetAnimInstance()->Montage_Play(ExpandMontage);
	CoreCaptureSpeed = FVector::Distance(GetMesh()->GetComponentLocation(), CoreToCapture->GetActorLocation())/ExpandDuration;

	CoreToCapture->SetActorEnableCollision(false);
	GetCharacterMovement()->MaxWalkSpeed = 0.f;

	FTimerHandle ExpandTimerHandle;
	GetWorldTimerManager().SetTimer(ExpandTimerHandle, this, &ThisClass::ExpandFinished, ExpandDuration);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AStormCore::ExpandFinished()
{
	CoreToCapture->SetActorLocation(GetMesh()->GetComponentLocation());
	CoreToCapture->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, Crunch::SocketName::Root);
	GetMesh()->GetAnimInstance()->Montage_Play(CaptureMontage);
}
