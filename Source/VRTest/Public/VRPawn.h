// VRPawn.h
// Il "corpo" del giocatore VR, porting C++ del VRPawn del template:
//   - VRRoot        : origine del tracking space (il (0,0,0) della stanza reale)
//   - Camera        : pilotata dall'HMD, non da codice
//   - Left/Right    : UMotionControllerComponent, seguono i controller
//   - Locomotion    : componente che implementa teleport + snap turn
//
// Il Pawn è fermo nel mondo: sono i componenti figli a muoversi nel
// tracking space. Teleportare = spostare il Pawn (l'origine), non la camera.

#pragma once

#include "CoreMinimal.h"
#include "GrabComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "LocomotionComponent.h"
#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

UCLASS()
class VRTEST_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	AVRPawn();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UMotionControllerComponent* LeftController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UMotionControllerComponent* RightController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	ULocomotionComponent* Locomotion;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* GrabLeftAction;   

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* GrabRightAction;  

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;     

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* TurnAction;     
	
	UPROPERTY(EditAnywhere, Category = "Grab")
	float GrabSearchRadius = 15.f;

protected:
	virtual void BeginPlay() override;

private:
	void OnGrabLeftStarted(const FInputActionValue& Value);
	void OnGrabLeftCompleted(const FInputActionValue& Value);
	void OnGrabRightStarted(const FInputActionValue& Value);
	void OnGrabRightCompleted(const FInputActionValue& Value);
	void OnMoveTriggered(const FInputActionValue& Value);
	void OnMoveCompleted(const FInputActionValue& Value);
	void OnTurnStarted(const FInputActionValue& Value);

	UGrabComponent* FindNearestGrabComponent(UMotionControllerComponent* Hand) const;

	void TryGrab(UMotionControllerComponent* Hand, UGrabComponent*& HeldSlot);
	void TryRelease(UGrabComponent*& HeldSlot);

	UPROPERTY()
	UGrabComponent* HeldLeft;

	UPROPERTY()
	UGrabComponent* HeldRight;
};