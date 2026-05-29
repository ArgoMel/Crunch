// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/TeamSelectionWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UTeamSelectionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SelectButton->OnClicked.AddDynamic(this, &ThisClass::SelectButtonClicked);
}

void UTeamSelectionWidget::SetSlotID(uint8 NewSlotID)
{
	SlotID = NewSlotID;
}

void UTeamSelectionWidget::UpdateSlotInfo(const FString& PlayerNickName) const
{
	InfoText->SetText(FText::FromString(PlayerNickName));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UTeamSelectionWidget::SelectButtonClicked()
{
	OnSlotClicked.Broadcast(SlotID);
}
