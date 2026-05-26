// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/CrosshairWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "GAS/CAbilitySystemStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UCrosshairWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CrosshairImage->SetVisibility(ESlateVisibility::Collapsed);

	UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerAbilitySystemComponent)
	{
		OwnerAbilitySystemComponent->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetCrosshairTag()).AddUObject(this, &ThisClass::CrosshairTagUpdated);
		OwnerAbilitySystemComponent->GenericGameplayEventCallbacks.Add(UCAbilitySystemStatics::GetTargetUpdatedTag()).AddUObject(this, &ThisClass::TargetUpdated);
	}
	CachedPlayerController = GetOwningPlayer();

	CrosshairCanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	if (!CrosshairCanvasPanelSlot)
	{
		UE_LOG(LogTemp, Error, TEXT("Crosshair widget need to be parented under a canvas panel to place itself properly"));
	}
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (CrosshairImage->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		UpdateCrosshairPosition();
	}
}

void UCrosshairWidget::CrosshairTagUpdated(const FGameplayTag Tag, int32 NewCount) const
{
	CrosshairImage->SetVisibility(NewCount > 0 ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UCrosshairWidget::UpdateCrosshairPosition() const
{
	if (!CachedPlayerController || !CrosshairCanvasPanelSlot)
	{
		return;
	}
	const float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
	int32 SizeX, SizeY;
	CachedPlayerController->GetViewportSize(SizeX, SizeY);
	if (!AimTarget)
	{
		const FVector2D ViewportSize = FVector2D{static_cast<float>(SizeX), static_cast<float>(SizeY)};
		CrosshairCanvasPanelSlot->SetPosition(ViewportSize / 2.f / ViewportScale);
		return;
	}

	FVector2D TargetScreenPosition;
	CachedPlayerController->ProjectWorldLocationToScreen(AimTarget->GetActorLocation(), TargetScreenPosition);
	if (TargetScreenPosition.X > 0 
		&& TargetScreenPosition.X < SizeX 
		&& TargetScreenPosition.Y > 0 
		&& TargetScreenPosition.Y < SizeY)
	{
		CrosshairCanvasPanelSlot->SetPosition(TargetScreenPosition / ViewportScale);
	}
}

void UCrosshairWidget::TargetUpdated(const FGameplayEventData* EventData)
{
	AimTarget = EventData->Target;
	CrosshairImage->SetColorAndOpacity(AimTarget ? HasTargetColor : NoTargetColor);
}
