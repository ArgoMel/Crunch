// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CCharacter.h"
#include "InputActionValue.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "CPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCHeroAttributeSet;
class UInventoryComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class ACPlayerCharacter : public ACCharacter
{
	GENERATED_BODY()
public:
	ACPlayerCharacter();
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	//pawn쪽 함수
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const  override;
	
private:
	virtual void OnAimStateChanged(bool bIsAiming) override;
	
private:
	FVector GetLookRightDir() const;
	FVector GetLookFwdDir() const;
	FVector GetMoveFwdDir() const;
	
	/*************************************************************/
	/*                           Input                           */
	/*************************************************************/
	void HandleLookInput(const FInputActionValue& InputActionValue);
	void HandleMoveInput(const FInputActionValue& InputActionValue);
	void LearnAbilityLeaderDown(const FInputActionValue& InputActionValue);
	void LearnAbilityLeaderUp(const FInputActionValue& InputActionValue);
	void UseInventoryItem(const FInputActionValue& InputActionValue);
	void HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID InputID);
	void SetInputEnabledFromPlayerController(bool bEnabled);

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	UCameraComponent* ViewCam;
	
	bool bIsLearnAbilityLeaderDown = false;
	
	/*************************************************************/
	/*                       Gameplay Ability                    */
	/*************************************************************/
	UPROPERTY()
	UCHeroAttributeSet* HeroAttributeSet;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpInputAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookInputAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveInputAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LearnAbilityLeaderAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* UseInventoryItemAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<ECAbilityInputID, UInputAction*> GameplayAbilityInputActions;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* GameplayInputMappingContext;
	
	/*************************************************************/
	/*                           Stun                            */
	/*************************************************************/
	virtual void OnStun() override;
	virtual void OnRecoverFromStun() override;
	/*************************************************************/
	/*                      Death and Respawn                    */
	/*************************************************************/

	virtual void OnDead() override;
	virtual void OnRespawn() override;
	/*************************************************************/
	/*                      Camera View                           */
	/*************************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category = view)
	FVector CameraAimLocalOffset;

	UPROPERTY(EditDefaultsOnly, Category = view)
	float CameraLerpSpeed = 20.f;
	
	FTimerHandle CameraLerpTimerHandle;

	void LerpCameraToLocalOffsetLocation(const FVector& Goal);
	void TickCameraLocalOffsetLerp(FVector Goal);


	/*************************************************************/
	/*                      Inventory                            */
	/*************************************************************/
private:
	UPROPERTY()
	UInventoryComponent* InventoryComponent;
};
