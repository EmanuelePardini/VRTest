// GrabComponent.h
// Porting C++ del GrabComponent del template.
// Filosofia: composition over inheritance — si AGGIUNGE questo
// componente come figlio di una StaticMesh con Simulate Physics
// e l'oggetto diventa afferrabile, senza sottoclassi per ogni attore.
//
// Sotto il cofano (le 3 mosse viste a lezione):
//   presa    -> SetSimulatePhysics(false) + AttachToComponent
//   tenuta   -> tick che campiona la velocità della mano
//   rilascio -> Detach + SetSimulatePhysics(true) + velocità ereditata

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "Components/SceneComponent.h"
#include "GrabComponent.generated.h"



UENUM(BlueprintType)
enum class EGrabType : uint8
{
	Free,
	Snap,
	Custom
};

UCLASS(ClassGroup = (VR), meta = (BlueprintSpawnableComponent))
class VRTEST_API UGrabComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UGrabComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab")
	EGrabType GrabType;
	UPROPERTY()
	UMotionControllerComponent* HoldingHand;
	UPROPERTY()
	bool IsHeld;
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void TryGrab(UMotionControllerComponent* Controller);
	void TryRelease();
	FORCEINLINE UGrabComponent* Get() { return this; }
};