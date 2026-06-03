// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/WaitingWidget.h"
#include "Components/TextBlock.h"

FOnButtonClickedEvent& UWaitingWidget::ClearAndGetButtonClickedEvent() const
{
	CancelBtn->OnClicked.Clear();
	return CancelBtn->OnClicked;
}

void UWaitingWidget::SetWaitInfo(const FText& WaitInfo, bool bAllowCancel) const
{
	if (CancelBtn)
	{
		CancelBtn->SetVisibility(bAllowCancel ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (WaitInfoText)
	{
		WaitInfoText->SetText(WaitInfo);
	}
}
