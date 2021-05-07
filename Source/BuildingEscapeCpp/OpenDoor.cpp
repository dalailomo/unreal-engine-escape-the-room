// Copyright 2020 example here. Set from project settings

#include "OpenDoor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	// Protect against null pointers because if not unreal engine crashes
	if (!PressurePlate) UE_LOG(LogTemp, Error, TEXT("Actor '%s' has no pressure plate set!!!"), *GetOwner()->GetName());
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() > MassToOpenDoor)
	{
		DoorLastOpenedTimestamp = GetWorld()->GetTimeSeconds();
		OpenDoor(DeltaTime);
	}
	else 
	{
		if (GetWorld()->GetTimeSeconds() - DoorLastOpenedTimestamp > DoorCloseDelay) CloseDoor(DeltaTime);
	}
}

/*
	NOTES:
	OpenDoorRotator.Yaw = FMath::Lerp(CurrentYaw, TargetYaw, 0.05f); // This is exponential interpolation, tied to the framerate
	OpenDoorRotator.Yaw = FMath::FInterpConstantTo(CurrentYaw, TargetYaw, DeltaTime, 45); // This is linear interpolation, independent of framerate
	DoorRotation.Yaw = FMath::FInterpTo(CurrentYaw, TargetYaw, DeltaTime, 2); // This is exponential interpolation, independent of framerate
*/

void UOpenDoor::OpenDoor(float DeltaTime)
{
	FRotator DoorRotation{ 0.f, OpenAngle, 0.f };
	RotateDoor(DeltaTime, DoorRotation, 2.f);
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	FRotator DoorRotation{ 0.f, InitialYaw, 0.f };
	RotateDoor(DeltaTime, DoorRotation, 4.f);
}

void UOpenDoor::RotateDoor(float DeltaTime, FRotator DoorRotation, float InterpolationSpeed)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, DoorRotation.Yaw, DeltaTime, InterpolationSpeed);
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}

float UOpenDoor::TotalMassOfActors() const
{	
	float TotalMass = 0.f;

	if (!PressurePlate) { return TotalMass; }

	// Find all overlapping actors
	TArray<AActor*> OverlappingActors;
	
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	
	// Add up their masses
	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;
}
