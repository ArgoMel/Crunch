// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/CAIController.h"
#include "Character/CCharacter.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Crunch/Crunch.h"
#include "GAS/CAbilitySystemStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ACAIController::ACAIController()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;
	SightConfig->SetMaxAge(5.f); //기억하는 시간
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::TargetPerceptionUpdated);
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::TargetForgotten);
}

void ACAIController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	const IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(NewPawn);
	if (PawnTeamInterface)
	{
		SetGenericTeamId(PawnTeamInterface->GetGenericTeamId());
		ClearAndDisableAllSenses();
		EnableAllSenses();
	}

	UAbilitySystemComponent* PawnASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(NewPawn);
	if (PawnASC)
	{
		PawnASC->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetDeadStatTag()).AddUObject(this, &ThisClass::PawnDeadTagUpdated);
		PawnASC->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetStunStatTag()).AddUObject(this, &ThisClass::PawnStunTagUpdated);
	}
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(BehaviorTree);
}

void ACAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (!GetCurrentTarget())
		{
			SetCurrentTarget(TargetActor);
		}
	}
	else
	{
		ForgetActorIfDead(TargetActor);
	}
}

void ACAIController::TargetForgotten(AActor* ForgottenActor)
{
	if (!ForgottenActor)
	{
		return;
	}
	if (GetCurrentTarget() == ForgottenActor)
	{
		SetCurrentTarget(GetNextPerceivedActor());
	}
}

const UObject* ACAIController::GetCurrentTarget() const
{
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent)
	{
		return GetBlackboardComponent()->GetValueAsObject(Crunch::AIKey::Target);
	}
	return nullptr;
}

void ACAIController::SetCurrentTarget(AActor* NewTarget)
{
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}
	if (NewTarget)
	{
		BlackboardComponent->SetValueAsObject(Crunch::AIKey::Target, NewTarget);
	}
	else
	{
		BlackboardComponent->ClearValue(Crunch::AIKey::Target);
	}
}

AActor* ACAIController::GetNextPerceivedActor() const
{
	if (PerceptionComponent)
	{
		TArray<AActor*> Actors;
		AIPerceptionComponent->GetPerceivedHostileActors(Actors);

		if (!Actors.IsEmpty())
		{
			return Actors[0];
		}
	}

	return nullptr;
}

void ACAIController::ForgetActorIfDead(AActor* ActorToForget) const
{
	const UAbilitySystemComponent* ActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToForget);
	if (!ActorASC)
	{
		return;
	}
	if (ActorASC->HasMatchingGameplayTag(UCAbilitySystemStatics::GetDeadStatTag()))
	{
		for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator Iter = AIPerceptionComponent->GetPerceptualDataIterator(); Iter; ++Iter)
		{
			if (Iter->Key != ActorToForget)
			{
				continue;
			}

			for (FAIStimulus& Stimuli : Iter->Value.LastSensedStimuli)
			{
				Stimuli.SetStimulusAge(TNumericLimits<float>::Max());
			}
		}
	}
}

void ACAIController::ClearAndDisableAllSenses()
{
	AIPerceptionComponent->AgeStimuli(TNumericLimits<float>::Max());

	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), false);
	}

	if (GetBlackboardComponent())
	{
		GetBlackboardComponent()->ClearValue(Crunch::AIKey::Target);
	}
}

void ACAIController::EnableAllSenses() const
{
	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), true);
	}
}

void ACAIController::PawnDeadTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (Count != 0)
	{
		GetBrainComponent()->StopLogic("Dead");
		ClearAndDisableAllSenses();
		bIsPawnDead = true;
	}
	else
	{
		GetBrainComponent()->StartLogic();
		EnableAllSenses();
		bIsPawnDead = false;
	}
}

void ACAIController::PawnStunTagUpdated(const FGameplayTag Tag, int32 Count) const
{
	if (bIsPawnDead)
	{
		return;
	}
	if (Count != 0)
	{
		GetBrainComponent()->StopLogic("Stun");
	}
	else
	{
		GetBrainComponent()->StartLogic();
	}
}



