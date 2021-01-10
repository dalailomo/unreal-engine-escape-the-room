// Copyright 2020 example here. Set from project settings


#include "OpenDoor.h"
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
	TargetYaw += InitialYaw;
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	FRotator DoorRotation { 0.f, TargetYaw, 0.f };
	CurrentYaw = GetOwner()->GetActorRotation().Yaw;

	if (CurrentYaw >= TargetYaw - 1.f) return;

	UE_LOG(LogTemp, Warning, TEXT("Rotation is: %s"), *GetOwner()->GetActorRotation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Rotation yaw is: %f"), GetOwner()->GetActorRotation().Yaw);

	//OpenDoorRotator.Yaw = FMath::Lerp(CurrentYaw, TargetYaw, 0.05f); // This is exponential interpolation, tied to the framerate
	//OpenDoorRotator.Yaw = FMath::FInterpConstantTo(CurrentYaw, TargetYaw, DeltaTime, 45); // This is linear interpolation, independent of framerate
	DoorRotation.Yaw = FMath::FInterpTo(CurrentYaw, TargetYaw, DeltaTime, 2); // This is exponential interpolation, independent of framerate

	GetOwner()->SetActorRotation(DoorRotation);
}
