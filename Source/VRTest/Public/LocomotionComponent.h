// LocomotionComponent.h
// Porting C++ della locomozione del template: teleport in 4 fasi
// (mira -> validazione -> anteprima -> salto) + snap turn.
//
//   1. Mira        : PredictProjectilePath dal MotionController
//   2. Validazione : ProjectPointToNavigation sulla NavMesh
//   3. Anteprima   : arco e anello disegnati in debug (verde/rosso)
//   4. Salto       : SetActorLocation del Pawn, preservando l'offset
//                    della testa nel play space
//
// La visualizzazione usa DrawDebug per restare senza dipendenze:
// sostituirla con SplineMesh + Niagara è un'estensione proposta.

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "LocomotionComponent.generated.h"


UCLASS(ClassGroup = (VR), meta = (BlueprintSpawnableComponent))
class VRTEST_API ULocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULocomotionComponent();
	
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float ProjectileSpeed = 900.f;
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float ProjectileRadius = 3.f;
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float MaxSimTime = 2.f;
	UPROPERTY(EditAnywhere, Category = "Teleport")
	FVector NavProjectionExtent = FVector(100.f, 100.f, 200.f);
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float AimThreshold = 0.6f;
	UPROPERTY(EditAnywhere, Category = "Turn")
	float SnapTurnDegrees = 45.f;
	
	void UpdateTeleportAim(UMotionControllerComponent* AimHand, const FVector2D& StickValue);
	void ConfirmTeleport();
	void SnapTurn(float Sign);

protected:
	virtual void BeginPlay() override;

private:
	void ComputeAim(UMotionControllerComponent* AimHand);
	void ExecuteTeleport();
	bool bIsAiming = false;
	bool bHasValidTarget = false;
	FVector TargetLocation = FVector::ZeroVector;
	UPROPERTY()
	UCameraComponent* Camera;
};