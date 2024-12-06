// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UShipMovementComponent::UShipMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UShipMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UShipMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShouldCreateNewMove()) {
		LastMove = CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}
}

void UShipMovementComponent::SetThrottle(float ThrottleValue)
{
	this->Throttle = ThrottleValue;
}

void UShipMovementComponent::SetPitch(float PitchValue)
{
	this->Pitch = PitchValue;
}

void UShipMovementComponent::SetYaw(float YawValue)
{
	this->Yaw = YawValue;
}

void UShipMovementComponent::SetRoll(float RollValue)
{
	this->Roll = RollValue;
}

void UShipMovementComponent::SimulateMove(FShipMove Move)
{

	CalculateForwardVelocity(Move);
	CalculateAngularVelocity(Move);

	UpdateLocationFromVelocity(Move);

}

void UShipMovementComponent::UpdateLocationFromVelocity(FShipMove& Move)
{
	FVector Translation = Velocity * Move.DeltaTime * 100;

	FHitResult CollisionResult;
	
	GetOwner()->AddActorWorldOffset(Translation, true, &CollisionResult);
	if (CollisionResult.IsValidBlockingHit()) {
		UE_LOG(LogTemp, Warning, TEXT("Blocking Hit"));
		Velocity = FVector::Zero();
	}
}

void UShipMovementComponent::CalculateForwardVelocity(FShipMove Move)
{
	if(Move.Throttle != 0){
		FVector force = GetOwner()->GetActorForwardVector() * MaxThrusterForce * Move.Throttle;
		FVector acceleration = force / Mass;
		Velocity += acceleration * Move.DeltaTime;
	}
}

void UShipMovementComponent::CalculateAngularVelocity(FShipMove Move)
{

	float rollForce = MaxRollForce * Move.Roll * Move.DeltaTime;
	float rollAcceleration = rollForce / Mass;
	FQuat RollDelta(GetOwner()->GetActorForwardVector(), rollAcceleration);
	GetOwner()->AddActorWorldRotation(RollDelta);

	float DeltaLocation = Velocity.Dot(GetOwner()->GetActorForwardVector()) * Move.DeltaTime;
	float PitchAngle = DeltaLocation / MinPitchRadius * Move.Pitch;
	FQuat PitchDelta(GetOwner()->GetActorRightVector(), PitchAngle);
	Velocity = PitchDelta.RotateVector(Velocity);
	GetOwner()->AddActorWorldRotation(PitchDelta);

	float YawAngle = DeltaLocation / MinYawRadius * Move.Yaw;
	FQuat YawDelta(GetOwner()->GetActorUpVector(), YawAngle);
	Velocity = YawDelta.RotateVector(Velocity);
	GetOwner()->AddActorWorldRotation(YawDelta);
}

FShipMove UShipMovementComponent::CreateMove(float DeltaTime)
{
	FShipMove newMove;
	newMove.Throttle = Throttle;
	newMove.Pitch = Pitch;
	newMove.Yaw = Yaw;
	newMove.Roll = Roll;
	newMove.DeltaTime = DeltaTime;
	newMove.Time = GetWorld()->TimeSeconds;

	return newMove;
}

bool UShipMovementComponent::ShouldCreateNewMove()
{
	if(GetOwnerRole() <= ROLE_SimulatedProxy) return false; //Should not create moves if role is simulated or none

	APawn* Owner = Cast<APawn>(GetOwner()); // Need to get owner as pawn because GetOwner->GetRemoteRole() returns inconsistant results
	if (Owner == nullptr) return false;

	APlayerController* playerController = Cast<APlayerController>(Owner->GetController());
	bool isPlayerControlled = playerController != nullptr;
	if (isPlayerControlled) {
		return (GetOwnerRole() == ROLE_AutonomousProxy) || (ROLE_Authority && Owner->IsLocallyControlled()); //Check if we are client or server as player
	}
	else {
		return true; //If no player is controlling, the ship should move on it's own
	}
}

