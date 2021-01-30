// Copyright 2020 example here. Set from project settings

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"

struct PlayerLineTraceEndMeta {
	FVector PlayerViewpointLocation;
	FRotator PlayerViewpointRotation;
	FVector LineTraceEnd;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPECPP_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

	// Called every frame
	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction
	) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	float Reach = 100.f;

	UInputComponent* InputComponent = nullptr;
	UPhysicsHandleComponent* PhysicsHandle = nullptr;

	void Grab();
	void Release();

	void FindPhysicsHandle();
	void SetupInputComponent();

	/* 
	Return the first Actor within reach with physics body
	The reason is const, is because we dont want the object we are hitting
	to change while at the same time we are still trying to access it and do
	something with it
	*/
	FHitResult GetFirstPhysicsBodyInReach() const;

	PlayerLineTraceEndMeta GetPlayerLineTraceEndMeta() const;
};
