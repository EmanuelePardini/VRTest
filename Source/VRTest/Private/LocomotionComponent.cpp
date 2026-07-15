// LocomotionComponent.cpp

#include "LocomotionComponent.h"

#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"

ULocomotionComponent::ULocomotionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULocomotionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Camera = GetOwner()->FindComponentByClass<UCameraComponent>();
}

void ULocomotionComponent::UpdateTeleportAim(UMotionControllerComponent* AimHand, const FVector2D& StickValue)
{
	if (!AimHand) return;

	if (StickValue.Y >= AimThreshold)
	{
		bIsAiming = true;
		ComputeAim(AimHand);
	}
	else if (bIsAiming)
	{
		bIsAiming = false;
		bHasValidTarget = false;
	}
}

void ULocomotionComponent::ComputeAim(UMotionControllerComponent* AimHand)
{

	// https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/FPredictProjectilePathParams/__ctor
	FPredictProjectilePathParams Params(
	   ProjectileRadius,
	   AimHand->GetComponentLocation(),
	   AimHand->GetForwardVector() * ProjectileSpeed,
	   MaxSimTime,
	   ECC_WorldStatic,
	   GetOwner());
	Params.bTraceWithCollision = true;
	Params.SimFrequency = 20.f;
	//Andiamo a settare i parametri della traiettoria che vogliamo simulare

	//Passata come reference alla funzione sotto restituisce un array di punti corrispondenti alla traiettoria
	// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/Kismet/FPredictProjectilePathResult
	FPredictProjectilePathResult Result;

	// UGameplayStatics::PredictProjectilePath simula l'arco del proiettile
	// soggetto a gravità con collision check lungo il percorso; true se colpisce qualcosa
	// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/Kismet/UGameplayStatics/PredictProjectilePath/1
	const bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, Result);

	// Check navmesh
	bHasValidTarget = false;
	if (bHit)
	{
		if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
		{
			FNavLocation NavLocation;
			if (Nav->ProjectPointToNavigation(Result.HitResult.Location, NavLocation, NavProjectionExtent))
			{
				bHasValidTarget = true;
				TargetLocation = NavLocation.Location;
			}
		}
	}
}

void ULocomotionComponent::ConfirmTeleport()
{
	if (bIsAiming && bHasValidTarget)
	{
		ExecuteTeleport();
	}
	bIsAiming = false;
	bHasValidTarget = false;
}

void ULocomotionComponent::ExecuteTeleport()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Camera) return;
	
	FVector HeadOffset = Camera->GetComponentLocation() - Owner->GetActorLocation();
	HeadOffset.Z = 0.f; 

	Owner->SetActorLocation(TargetLocation - HeadOffset);
}


void ULocomotionComponent::SnapTurn(float Sign)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Camera) return;

	const FRotator Delta(0.f, Sign * SnapTurnDegrees, 0.f);

	//Per evitare discontinuità visiva e nausea usiamo come pivot del player la camera
	const FVector ActorLocation = Owner->GetActorLocation();
	FVector Pivot = Camera->GetComponentLocation();
	Pivot.Z = ActorLocation.Z;

	const FVector Offset = Delta.RotateVector(ActorLocation - Pivot);
	Owner->SetActorLocation(Pivot + Offset);
	Owner->AddActorWorldRotation(Delta);
}