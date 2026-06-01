// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/CharacterEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Character/PA_CharacterDefinition.h"
#include "Crunch/Crunch.h"

void UCharacterEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	CharacterDefinition = Cast<UPA_CharacterDefinition>(ListItemObject);
	if (CharacterDefinition)
	{
		CharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(Crunch::MatParam::Icon, CharacterDefinition->LoadIcon());
		CharacterNameText->SetText(FText::FromString(CharacterDefinition->GetCharacterDisplayName()));
	}
}

void UCharacterEntryWidget::SetSelected(bool bIsSelected) const
{
	CharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(Crunch::MatParam::Saturation, bIsSelected ? 0.f : 1.f);
}
