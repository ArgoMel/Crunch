// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PA_CharacterDefinition.h"
#include "Character/CCharacter.h"

FPrimaryAssetId UPA_CharacterDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetCharacterDefinationAssetType(), GetFName());
}

FPrimaryAssetType UPA_CharacterDefinition::GetCharacterDefinationAssetType()
{
	return FPrimaryAssetType("CharacterDefination");
}

UTexture2D* UPA_CharacterDefinition::LoadIcon() const
{
	CharacterIcon.LoadSynchronous();
	if (CharacterIcon.IsValid())
		return CharacterIcon.Get();

	return nullptr;
}

TSubclassOf<ACCharacter> UPA_CharacterDefinition::LoadCharacterClass() const
{
	CharacterClass.LoadSynchronous();
	if (CharacterClass.IsValid())
		return CharacterClass.Get();
	
	return TSubclassOf<ACCharacter>();

}

TSubclassOf<UAnimInstance> UPA_CharacterDefinition::LoadDisplayAnimationBP() const
{
	DisplayAnimBP.LoadSynchronous();
	if (DisplayAnimBP.IsValid())
		return DisplayAnimBP.Get();

	return TSubclassOf<UAnimInstance>();
}

USkeletalMesh* UPA_CharacterDefinition::LoadDisplayMesh() const
{
	const TSubclassOf<ACCharacter> LoadedCharaterClass = LoadCharacterClass();
	if (!LoadedCharaterClass)
		return nullptr;

	const ACharacter* Character = Cast<ACharacter>(LoadedCharaterClass.GetDefaultObject());
	if (!Character)
		return nullptr;

	return Character->GetMesh()->GetSkeletalMeshAsset();
}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>* UPA_CharacterDefinition::GetAbilities() const
{
	const TSubclassOf<ACCharacter> LoadedCharaterClass = LoadCharacterClass();
	if (!LoadedCharaterClass)
		return nullptr;

	const ACCharacter* Character = Cast<ACCharacter>(LoadedCharaterClass.GetDefaultObject());
	if (!Character)
		return nullptr;

	return &(Character->GetAbilities());
}

