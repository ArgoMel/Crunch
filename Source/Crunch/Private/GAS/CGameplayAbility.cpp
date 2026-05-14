// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GAS/GAP_Launched.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GameFramework/Character.h"

UCGameplayAbility::UCGameplayAbility()
{
	ActivationBlockedTags.AddTag(UCAbilitySystemStatics::GetStunStatTag());
}

bool UCGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	const FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (AbilitySpec && AbilitySpec->Level <= 0)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

AActor* UCGameplayAbility::GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const
{
	const AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
	if (OwnerAvatarActor)
	{
		FVector Location;
		FRotator Rotation;
		OwnerAvatarActor->GetActorEyesViewPoint(Location, Rotation);

		const FVector AimEnd = Location + Rotation.Vector() * AimDistance;

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(OwnerAvatarActor);
		
		FCollisionObjectQueryParams CollisionObjectQueryParams;
		CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		if (ShouldDrawDebug())
		{
			DrawDebugLine(GetWorld(), Location, AimEnd, FColor::Red, false, 2.f, 0U, 3.f);
		}

		TArray<FHitResult> HitResults;
		if (GetWorld()->LineTraceMultiByObjectType(HitResults, Location, AimEnd, CollisionObjectQueryParams, CollisionQueryParams))
		{
			for (FHitResult& HitResult : HitResults)
			{
				if (IsActorTeamAttitudeIs(HitResult.GetActor(), TeamAttitude))
				{
					return HitResult.GetActor();
				}
			}
		}
	}

	return nullptr;
}

UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
	const USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMeshComp)
	{
		return OwnerSkeletalMeshComp->GetAnimInstance();
	}
	return nullptr;
}

void UCGameplayAbility::PushSelf(const FVector& PushVel)
{
	ACharacter* OwningAvatarCharacter = GetOwningAvatarCharacter();
	if (OwningAvatarCharacter)
	{
		OwningAvatarCharacter->LaunchCharacter(PushVel, true, true);
	}
}

void UCGameplayAbility::PushTarget(AActor* Target, const FVector& PushVel) const
{
	if (!Target)
	{
		return;
	}
	FGameplayEventData EventData;

	FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit;
	FHitResult HitResult;
	HitResult.ImpactNormal = PushVel;
	HitData->HitResult = HitResult;
	EventData.TargetData.Add(HitData);
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target,UGAP_Launched::GetLaunchedAbilityActivationTag(), EventData);
}

void UCGameplayAbility::PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel) const
{
	for(AActor* Target : Targets)
	{
		PushTarget(Target, PushVel);
	}
}

void UCGameplayAbility::PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVel) const
{
	const TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargets(Targets, PushVel);
}

void UCGameplayAbility::PushTargetsFromOwnerLocation(const TArray<AActor*>& Targets, float PushSpeed)
{
	const AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
	if (!OwnerAvatarActor)
		return;

	const FVector OwnerAvatarActorLocation = OwnerAvatarActor->GetActorLocation();
	PushTargetsFromLocation(Targets, OwnerAvatarActorLocation, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromOwnerLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float PushSpeed)
{
	const TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargetsFromOwnerLocation(TargetActors, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& FromLocation, float PushSpeed)
{
	const TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargetsFromLocation(Targets, FromLocation, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromLocation(const TArray<AActor*>& Targets, const FVector& FromLocation, float PushSpeed)
{
	for (AActor* Target : Targets)
	{
		FVector PushDir = Target->GetActorLocation() - FromLocation;
		PushDir.Z = 0;
		PushDir.Normalize();

		PushTarget(Target, PushDir * PushSpeed);
	}
}

void UCGameplayAbility::PlayMontageLocally(UAnimMontage* MontageToPlay)
{
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (OwnerAnimInst && !OwnerAnimInst->Montage_IsPlaying(MontageToPlay))
	{
		OwnerAnimInst->Montage_Play(MontageToPlay);
	}
}

void UCGameplayAbility::StopMontageAfterCurrentSection(UAnimMontage* MontageToStop)
{
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (OwnerAnimInst)
	{
		const FName CurrentSectionName = OwnerAnimInst->Montage_GetCurrentSection(MontageToStop);
		OwnerAnimInst->Montage_SetNextSection(CurrentSectionName, NAME_None, MontageToStop);
	}
}

FGenericTeamId UCGameplayAbility::GetOwnerTeamId() const
{
	const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	if (OwnerTeamInterface)
	{
		return OwnerTeamInterface->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}

bool UCGameplayAbility::IsActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const
{
	if (!OtherActor)
		return false;

	const IGenericTeamAgentInterface* OwnerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	if (OwnerTeamAgentInterface)
	{
		return OwnerTeamAgentInterface->GetTeamAttitudeTowards(*OtherActor) == TeamAttitude;
	}

	return false;
}

ACharacter* UCGameplayAbility::GetOwningAvatarCharacter()
{
	if (!AvatarCharacter.IsValid())
	{
		AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	}

	return AvatarCharacter.Get();
}

void UCGameplayAbility::ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level) const
{
	const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect, Level);

	FGameplayEffectContextHandle EffectContext = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
	EffectContext.AddHitResult(HitResult);

	EffectSpecHandle.Data->SetContext(EffectContext);

	ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor()));
}

void UCGameplayAbility::SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData) const
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (OwnerASC)
	{
		OwnerASC->HandleGameplayEvent(EventTag, &EventData);
	}
}

