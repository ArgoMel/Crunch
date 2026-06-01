// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/PlayerTeamSlotWidget.h"
#include "Character/PA_CharacterDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Crunch/Crunch.h"

void UPlayerTeamSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(Crunch::MatParam::Empty, 1);
	CachedCharacterNameStr = "";
}

void UPlayerTeamSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	NameText->SetText(FText::FromString(CachedCharacterNameStr));
	PlayAnimationForward(HoverAnim);
}

void UPlayerTeamSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	NameText->SetText(FText::FromString(CachedPlayerNameStr));
	PlayAnimationReverse(HoverAnim);
}

void UPlayerTeamSlotWidget::UpdateSlot(const FString& PlayerName, const UPA_CharacterDefinition* CharacterDefinition)
{
	CachedPlayerNameStr = PlayerName;

	if (CharacterDefinition)
	{
		PlayerCharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(Crunch::MatParam::Icon, CharacterDefinition->LoadIcon());
		PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(Crunch::MatParam::Empty, 0);
		CachedCharacterNameStr = CharacterDefinition->GetCharacterDisplayName();
	}
	else
	{
		PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(Crunch::MatParam::Empty, 1);
		CachedCharacterNameStr = "";
	}

	UpdateNameText();
}

void UPlayerTeamSlotWidget::UpdateNameText() const
{
	if (IsHovered())
	{
		NameText->SetText(FText::FromString(CachedCharacterNameStr));
	}
	else
	{
		NameText->SetText(FText::FromString(CachedPlayerNameStr));
	}
}
