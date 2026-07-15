// VRPawn.cpp

#include "VRPawn.h"

#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LocomotionComponent.h"
#include "GrabComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"

//Ripasso dei fondamentali tramite VR:
//https://www.youtube.com/watch?v=nBKEMMbD3Pk

//Guida al setup di Meta
//https://developers.meta.com/horizon/documentation/unreal/unreal-create-and-configure-new-project/
AVRPawn::AVRPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	SetRootComponent(VRRoot);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);
	
	//Documentazione per l'utilizzo di UMotionControllerComponent
	//https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/HeadMountedDisplay/UMotionControllerComponent
	//https://developers.meta.com/horizon/documentation/unreal/unreal-hand-tracking/
	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	LeftController->SetupAttachment(VRRoot);
	LeftController->MotionSource = TEXT("Left");

	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	RightController->SetupAttachment(VRRoot);
	RightController->MotionSource = TEXT("Right");

	Locomotion = CreateDefaultSubobject<ULocomotionComponent>(TEXT("Locomotion"));
}

void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	//Documentazione interessante di Meta per il setup di un progetto Unreal in VR
	//https://developers.meta.com/horizon/documentation/unreal/unreal-engine-basics/
 	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::LocalFloor);

}

void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	
	if (GrabLeftAction)
	{
		EIC->BindAction(GrabLeftAction,  ETriggerEvent::Started,   this, &AVRPawn::OnGrabLeftStarted);
		EIC->BindAction(GrabLeftAction,  ETriggerEvent::Completed, this, &AVRPawn::OnGrabLeftCompleted);
	}
	if (GrabRightAction)
	{
		EIC->BindAction(GrabRightAction, ETriggerEvent::Started,   this, &AVRPawn::OnGrabRightStarted);
		EIC->BindAction(GrabRightAction, ETriggerEvent::Completed, this, &AVRPawn::OnGrabRightCompleted);
	}
	if (MoveAction)
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVRPawn::OnMoveTriggered);
		EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &AVRPawn::OnMoveCompleted);
	}
	if (TurnAction)
	{
		EIC->BindAction(TurnAction, ETriggerEvent::Started, this, &AVRPawn::OnTurnStarted);
	}
}

void AVRPawn::OnGrabLeftStarted(const FInputActionValue&)
{
	TryGrab(LeftController, HeldLeft);
}

void AVRPawn::OnGrabLeftCompleted(const FInputActionValue&)
{
	TryRelease(HeldLeft);
}

void AVRPawn::OnGrabRightStarted(const FInputActionValue&)
{
	TryGrab(RightController, HeldRight);
}

void AVRPawn::OnGrabRightCompleted(const FInputActionValue&)
{
	TryRelease(HeldRight);
}
														

void AVRPawn::TryGrab(UMotionControllerComponent* Hand, UGrabComponent*& HeldSlot) //Reference a puntatore
{													//la funzione che la riceve può riassegnare il puntatore originale del chiamante														
	//Per rivedere le reference:
	//https://en.cppreference.com/cpp/language/reference
	//https://www.geeksforgeeks.org/cpp/pointers-and-references-in-c/
	if (HeldSlot) return;
	
	UGrabComponent* Grab = FindNearestGrabComponent(Hand);
	if (Grab)
	{
		Grab->TryGrab(Hand);
		HeldSlot = Grab;
	}
}

void AVRPawn::TryRelease(UGrabComponent*& HeldSlot)
{
	if (HeldSlot) return;
	
	HeldSlot->TryRelease();
	HeldSlot = nullptr;
	
}

//Semplificato metodo con sphereTrace
UGrabComponent* AVRPawn::FindNearestGrabComponent(UMotionControllerComponent* Hand) const
{
	if (!Hand) return nullptr;

	const FVector HandLocation = Hand->GetComponentLocation();

	FHitResult Hit;
	UKismetSystemLibrary::SphereTraceSingle(
		this, HandLocation, HandLocation, GrabSearchRadius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		{ const_cast<AVRPawn*>(this) },
		EDrawDebugTrace::None, //Tipo di debug trace(La vediamo in Editor)
		Hit,
		true);

	if (!Hit.bBlockingHit || !Hit.GetActor()) return nullptr;

	return Hit.GetActor()->FindComponentByClass<UGrabComponent>();
}

void AVRPawn::OnMoveTriggered(const FInputActionValue& Value)
{
	const FVector2D Stick = Value.Get<FVector2D>();
	//Inizia il trace per il teletrasporto
	Locomotion->UpdateTeleportAim(RightController, Stick);
}

void AVRPawn::OnMoveCompleted(const FInputActionValue&)
{
	//Effettua il teletrasporto
	Locomotion->ConfirmTeleport();
}

void AVRPawn::OnTurnStarted(const FInputActionValue& Value)
{
	const float Axis = Value.Get<float>();
	if (FMath::Abs(Axis) > 0.5f)
	{	//Visto che fa uno snap e una rotazione "violente", controlliamo che l'input sia alto 
		//per non provocare falsi trigger
		Locomotion->SnapTurn(FMath::Sign(Axis));
	}
}