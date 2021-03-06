// Copyright 2020 example here. Set from project settings

#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grabber pressed"));

	// Raycast when key is pressed and try and reach any actors with physics body collision channel set
	FHitResult Target = GetFirstPhysicsBodyInReach();
	PlayerLineTraceEndMeta PlayerLineTrace = GetPlayerLineTraceEndMeta();
	UPrimitiveComponent* ComponentToGrab = Target.GetComponent();
	AActor* ActorHit = Target.GetActor();

	// If we hit something, then we attach the physics handle
	if (ActorHit)
	{
		if (!PhysicsHandle) { return; }

		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			PlayerLineTrace.LineTraceEnd
		);
	}
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grabber released"));
	
	if (!PhysicsHandle) { return; }

	PhysicsHandle->ReleaseComponent();
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicsHandle is not present for  %s"), *(GetOwner()->GetName()));
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		// Name has to be the same as set in project settings input bindings
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

PlayerLineTraceEndMeta UGrabber::GetPlayerLineTraceEndMeta() const
{
	// Get the players viewpoint
	FVector PlayerViewpointLocation;
	FRotator PlayerViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewpointLocation,
		OUT PlayerViewpointRotation
	);

	// Raycast out to a certain distance (Reach)
	FVector LineTraceEnd = PlayerViewpointLocation + PlayerViewpointRotation.Vector() * Reach;

	return { PlayerViewpointLocation, PlayerViewpointRotation, LineTraceEnd };
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	PlayerLineTraceEndMeta PlayerLineTrace = GetPlayerLineTraceEndMeta();
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerLineTrace.PlayerViewpointLocation,
		PlayerLineTrace.LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody)
	);

	// See what it hits
	AActor* ActorHit = Hit.GetActor();

	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *(ActorHit->GetName()));
	}

	return Hit;

	/* debugging sheit
	UE_LOG(LogTemp, Warning, TEXT("l:%s||r:%s"),
		*PlayerViewpointLocation.ToString(),
		*PlayerViewpointRotation.ToString()
	);

	// Draw a line from player showing the reach
	FVector LineTraceEnd = PlayerViewpointLocation + PlayerViewpointRotation.Vector() * Reach;

	DrawDebugLine(
		GetWorld(),
		PlayerViewpointLocation,
		LineTraceEnd,
		FColor(0, 255, 0),
		false,
		0.f,
		0,
		5.f
	);
	*/
}

// Called every frame
void UGrabber::TickComponent(
	float DeltaTime, 
	ELevelTick TickType, 
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }

	if (PhysicsHandle->GrabbedComponent)
	{
		PlayerLineTraceEndMeta PlayerLineTrace = GetPlayerLineTraceEndMeta();
		PhysicsHandle->SetTargetLocation(PlayerLineTrace.LineTraceEnd);
	}
}
