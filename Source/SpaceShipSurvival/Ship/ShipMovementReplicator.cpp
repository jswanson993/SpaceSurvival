// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipMovementReplicator.h"
#include "Net/UnrealNetwork.h"

#include "ShipMovementComponent.h"

// Sets default values for this component's properties
UShipMovementReplicator::UShipMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UShipMovementReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)  const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UShipMovementReplicator, ServerState);
}

// Called when the game starts
void UShipMovementReplicator::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UShipMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(MovementComponent == nullptr) return;

	FShipMove Move = MovementComponent->GetLastMove();
	
	if(GetOwnerRole() == ROLE_AutonomousProxy){
		UnacknowlegedMoves.Add(Move);
		Server_SendMove(Move);
	}

	if (GetOwnerRole() == ROLE_Authority) {
		UpdateServerState(Move);
	}

	if (GetOwnerRole() == ROLE_SimulatedProxy) {
		ClientTick(DeltaTime);
	}


	ClearAcknowledgedMoves(Move);
	
}

void UShipMovementReplicator::ClientTick(float DeltaTime)
{
	ClientTimeSinceLastUpdate += DeltaTime;

	if(ClientTimeBetweenLastUpdates < KINDA_SMALL_NUMBER) return;

	float lerpRatio = ClientTimeSinceLastUpdate/ClientTimeBetweenLastUpdates;
	float VelocityToDerivative = ClientTimeBetweenLastUpdates * 100; //Convert from meters per second to centimeters per second

	FHermiteCubicSpline spline = CreateSpline(VelocityToDerivative);

	InterpolateLocation(spline, lerpRatio);
	InterpolateVelocity(spline, lerpRatio);
	InterpolateRotation(lerpRatio);

}

void UShipMovementReplicator::Server_SendMove_Implementation(FShipMove Move)
{
	if(MovementComponent == nullptr) return;
	ClientSimulatedTime += Move.DeltaTime;
	MovementComponent->SimulateMove(Move);
	UpdateServerState(Move);
}

bool UShipMovementReplicator::Server_SendMove_Validate(FShipMove Move) {
	return true;
}

void UShipMovementReplicator::UpdateServerState(const FShipMove Move)
{
	if(MovementComponent == nullptr) return;

	ServerState.Velocity = MovementComponent->GetVelocity();
	ServerState.Transform = GetOwner()->GetTransform();
	ServerState.LastMove = Move;
}

void UShipMovementReplicator::ClearAcknowledgedMoves(FShipMove LastMove)
{
	TArray<FShipMove> NewMoves;

	for(const FShipMove& move : UnacknowlegedMoves)
	{
		if (move.Time > LastMove.Time) {
			NewMoves.Add(move);
		}
	}

	UnacknowlegedMoves = NewMoves;
}

void UShipMovementReplicator::OnRep_ServerState()
{
	if (GetOwnerRole() == ROLE_AutonomousProxy) {
		AutonomousProxy_OnRep_ServerState();
	}
	else if (GetOwnerRole() == ROLE_SimulatedProxy) {
		SimulatedProxy_OnRep_ServerState();
	}
}

void UShipMovementReplicator::AutonomousProxy_OnRep_ServerState()
{
	if (MovementComponent == nullptr) return;

	GetOwner()->SetActorTransform(ServerState.Transform);
	MovementComponent->SetVelocity(ServerState.Velocity);
	ClearAcknowledgedMoves(ServerState.LastMove);

	for (const FShipMove& Move : UnacknowlegedMoves) {
		MovementComponent->SimulateMove(Move);
	}
}

void UShipMovementReplicator::SimulatedProxy_OnRep_ServerState()
{
	if(MovementComponent == nullptr) return;

	ClientTimeBetweenLastUpdates = ClientTimeSinceLastUpdate;
	ClientTimeSinceLastUpdate = 0;

	ClientStartTransform.SetLocation(GetOwner()->GetActorLocation());
	ClientStartTransform.SetRotation(GetOwner()->GetActorQuat());
	ClientStartVelocity = MovementComponent->GetVelocity();

	GetOwner()->SetActorTransform(ServerState.Transform);

}

FHermiteCubicSpline UShipMovementReplicator::CreateSpline(float VelocityToDerivative)
{
	FHermiteCubicSpline spline;
	spline.StartLocation = ClientStartTransform.GetLocation();
	spline.TargetLocation = ServerState.Transform.GetLocation();
	spline.StartDerivative = ClientStartVelocity * VelocityToDerivative;
	spline.TargetDerivative = ServerState.Velocity * VelocityToDerivative;
	return FHermiteCubicSpline();
}

void UShipMovementReplicator::InterpolateLocation(FHermiteCubicSpline Spline, float LerpRatio)
{
	FVector nextLocation = Spline.InterpolateLocation(LerpRatio);
	GetOwner()->SetActorLocation(nextLocation);
}

void UShipMovementReplicator::InterpolateRotation(float LerpRatio)
{
	FQuat targetRotation = ServerState.Transform.GetRotation();
	FQuat nextRotation = FQuat::Slerp(ClientStartTransform.GetRotation(), targetRotation, LerpRatio);
	GetOwner()->SetActorRotation(nextRotation);

}

void UShipMovementReplicator::InterpolateVelocity(FHermiteCubicSpline Spline, float LerpRatio)
{
	FVector nextDerivative = Spline.InterpolateDerivative(LerpRatio);
	FVector nextVelocity = nextDerivative / ServerState.Velocity;

	MovementComponent->SetVelocity(nextVelocity);
	
}

