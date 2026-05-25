// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/GameplayMenu.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/TextBlock.h"

void UGameplayMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	MainMenuBtn->OnClicked.AddDynamic(this, &ThisClass::BackToMainMenu);
	QuitGameBtn->OnClicked.AddDynamic(this, &ThisClass::QuitGame);
}

FOnButtonClickedEvent& UGameplayMenu::GetResumeButtonClickedEventDelegate() const
{
	return ResumeBtn->OnClicked;
}

void UGameplayMenu::SetTitleText(const FString& NewTitle) const
{
	MenuTitle->SetText(FText::FromString(NewTitle));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGameplayMenu::BackToMainMenu()
{

}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGameplayMenu::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
