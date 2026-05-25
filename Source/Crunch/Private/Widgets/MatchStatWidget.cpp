// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MatchStatWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Crunch/Crunch.h"
#include "Framework/StormCore.h"
#include "Kismet/GameplayStatics.h"

void UMatchStatWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	StormCore = Cast<AStormCore>(UGameplayStatics::GetActorOfClass(this, AStormCore::StaticClass()));
	if (StormCore.IsValid())
	{
		StormCore->OnTeamInfluenceCountUpdated.AddUObject(this, &ThisClass::UpdateTeamInfluence);
		StormCore->OnGoalReachedDelegate.AddUObject(this, &ThisClass::MatchFinished);
		GetWorld()->GetTimerManager().SetTimer(UpdateProgressTimerHandle, this, &ThisClass::UpdateProgress, ProgressUpdateInterval, true);
	}
}

void UMatchStatWidget::UpdateTeamInfluence(int TeamOneCount, int TeamTwoCount) const
{
	TeamOneCountText->SetText(FText::AsNumber(TeamOneCount));
	TeamTwoCountText->SetText(FText::AsNumber(TeamTwoCount));
}

void UMatchStatWidget::MatchFinished(AActor* ViewTarget, int WinningTeam)
{
	const float Progress = WinningTeam == 0 ? 1 : 0;

	GetWorld()->GetTimerManager().ClearTimer(UpdateProgressTimerHandle);
	ProgressImage->GetDynamicMaterial()->SetScalarParameterValue(Crunch::MatParam::Progress, Progress);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UMatchStatWidget::UpdateProgress()
{
	if (StormCore.IsValid())
	{
		const float Progress = StormCore->GetProgress();
		ProgressImage->GetDynamicMaterial()->SetScalarParameterValue(Crunch::MatParam::Progress, Progress);
	}
}
