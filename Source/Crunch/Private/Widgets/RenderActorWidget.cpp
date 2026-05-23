// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/RenderActorWidget.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Crunch/Crunch.h"
#include "Widgets/RenderActor.h"
#include "Engine/TextureRenderTarget2D.h"

void URenderActorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SpawnRenderActor();
	ConfigureRenderActor();
	BeginRenderCapture();
}

void URenderActorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (RenderSizeBox)
	{
		RenderSizeBox->SetWidthOverride(RenderSize.X);
		RenderSizeBox->SetHeightOverride(RenderSize.Y);
	}
}

void URenderActorWidget::NativeDestruct()
{
	Super::NativeDestruct();
	StopRenderCapture();
}

void URenderActorWidget::ConfigureRenderActor()
{
	if (!GetRenderActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Render Actor, Nothing will be rendered"));
		return;
	}

	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(static_cast<int>(RenderSize.X), static_cast<int>(RenderSize.Y));
	RenderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
	GetRenderActor()->SetRenderTarget(RenderTarget);

	UMaterialInstanceDynamic* DisplayImageDynamicMaterial = DisplayImage->GetDynamicMaterial();
	if (DisplayImageDynamicMaterial)
	{
		DisplayImageDynamicMaterial->SetTextureParameterValue(Crunch::MatParam::RenderTarget, RenderTarget);
	}
}

void URenderActorWidget::BeginRenderCapture()
{
	RenderTickInterval = 1.f / static_cast<float>(FrameRate);
	const UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(RenderTimerHandle, this, &ThisClass::UpdateRender, RenderTickInterval, true);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void URenderActorWidget::UpdateRender()
{
	if (GetRenderActor())
	{
		GetRenderActor()->UpdateRender();
	}
}

void URenderActorWidget::StopRenderCapture()
{
	const UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(RenderTimerHandle);
	}
}
