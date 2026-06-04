// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionEntryWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionEntrySelected, const FString& /*SelectedSessionIdStr*/)

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class USessionEntryWidget : public UUserWidget
{
	GENERATED_BODY()
protected:	
	virtual void NativeOnInitialized() override;
	
public:	
	void InitializeEntry(const FString& Name, const FString& SessionIdStr);
	FORCEINLINE FString GetCachedSessionIdStr() const { return CachedSessionIdStr; }
	
private:
	UFUNCTION()
	void SessionEntrySelected();
	
public:	
	FOnSessionEntrySelected OnSessionEntrySelected;
	
private:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SessionNameText;

	UPROPERTY(meta=(BindWidget))
	UButton* SessionBtn;

	FString CachedSessionIdStr;
};
