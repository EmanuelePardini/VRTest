// GrabComponent.cpp

#include "GrabComponent.h"

#include "MotionControllerComponent.h"
#include "Components/PrimitiveComponent.h"

UGrabComponent::UGrabComponent()
{
}

void UGrabComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGrabComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGrabComponent::TryGrab(UMotionControllerComponent* Controller)
{
	if (!HoldingHand || IsHeld) return;
	//UPrimitiveComponent eredita da SceneComponent, ma è lui che aggiunge geometria e collisione
	//Per azioni sulla fisica dobbiamo quindi upcastare a lui
	UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(GetAttachParent());
	
	Primitive->SetSimulatePhysics(false);
	switch (GrabType)
	{
	case EGrabType::Free:
		//Per lo studio dell'attachment:
		//https://www.reddit.com/r/unrealengine/comments/5l89oc/question_when_using_attachtocomponent_what_do_the/
		//https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/FAttachmentTransformRules
		Primitive->AttachToComponent(Controller, FAttachmentTransformRules::KeepWorldTransform);
		break;
	// case EGrabType::Snap:
	// 	TODO: Implementare
	}
}

void UGrabComponent::TryRelease()
{
	//TODO:Implementare
}
