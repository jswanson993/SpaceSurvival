// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipMovementReplicator.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UShipMovementReplicator::UShipMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UShipMovementReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)  const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UShipMovementReplicator, ServerState);
}

// Called when the game starts
void UShipMovementReplicator::BeginPlay()
{
	Super::BeginPlay();
	
	MovementComponent = GetOwner()->FindComponentByClass<UShipMovementComponent>();
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

	if(ShouldUpdateServer()){
		UpdateServerState(Move);
	}

	if (GetOwnerRole() == ROLE_SimulatedProxy) {
		ClientTick(DeltaTime);
	}
}

void UShipMovementReplicator::ClientTick(float DeltaTime)
{
	ClientTimeSinceLastUpdate += DeltaTime;

	if(ClientTimeBetweenLastUpdates < KINDA_SMALL_NUMBER) return;

	if(MovementComponent == nullptr) return;
	float lerpRatio =  ClientTimeSinceLastUpdate / ClientTimeBetweenLastUpdates;

	FHermiteCubicSpline spline = CreateSpline();

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
	
	float ProposedTime = ClientSimulatedTime + Move.DeltaTime;
	bool ClientNotRunningAhead = ProposedTime < GetWorld()->TimeSeconds;
	if (!ClientNotRunningAhead) {
		UE_LOG(LogTemp, Error, TEXT("Client is running too fast."))
			return false;
	}

	if (!Move.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Received invalid move."))
			return false;
	}
	
	return true;
}

void UShipMovementReplicator::UpdateServerState(const FShipMove& Move)
{
	if(MovementComponent == nullptr) return;
	ServerState.Velocity = MovementComponent->GetVelocity();
	ServerState.Transform = GetOwner()->GetActorTransform();
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

	for (const FShipMove& Move : UnacknowlegedMoves) 
	{
		MovementComponent->SimulateMove(Move);
	}
}

void UShipMovementReplicator::SimulatedProxy_OnRep_ServerState()
{
	if(MovementComponent == nullptr) return;
	ClientTimeBetweenLastUpdates = ClientTimeSinceLastUpdate;
	ClientTimeSinceLastUpdate = 0;

	if(MeshOffsetRoot != nullptr)
	{
		ClientStartTransform.SetLocation(MeshOffsetRoot->GetComponentLocation());
		ClientStartTransform.SetRotation(MeshOffsetRoot->GetComponentQuat());
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find Mesh Offset root"));
		ClientStartTransform.SetLocation(GetOwner()->GetActorLocation());
		ClientStartTransform.SetRotation(GetOwner()->GetActorQuat());
	}

	ClientStartVelocity = MovementComponent->GetVelocity();
	
	GetOwner()->SetActorTransform(ServerState.Transform);
	MeshOffsetRoot->SetWorldTransform(ClientStartTransform);
}

FHermiteCubicSpline UShipMovementReplicator::CreateSpline()
{
	FHermiteCubicSpline spline;
	spline.StartLocation = ClientStartTransform.GetLocation();
	spline.TargetLocation = ServerState.Transform.GetLocation();
	spline.StartDerivative = ClientStartVelocity * VelocityToDerivative();
	spline.TargetDerivative = ServerState.Velocity * VelocityToDerivative();
	return spline;
}

void UShipMovementReplicator::InterpolateLocation(FHermiteCubicSpline &Spline, float LerpRatio)
{
	FVector nextLocation = Spline.InterpolateLocation(LerpRatio);
	if(MeshOffsetRoot != nullptr){
		//UE_LOG(LogTemp, Error, TEXT("Next Location: %s"), *nextLocation.ToString())
		MeshOffsetRoot->SetWorldLocation(nextLocation);
	}else{
		GetOwner()->SetActorLocation(nextLocation);
	}
}

void UShipMovementReplicator::InterpolateVelocity(FHermiteCubicSpline &Spline, float LerpRatio)
{
	FVector nextDerivative = Spline.InterpolateDerivative(LerpRatio);
	FVector nextVelocity = nextDerivative / VelocityToDerivative();
	MovementComponent->SetVelocity(nextVelocity);
}

void UShipMovementReplicator::InterpolateRotation(float LerpRatio)
{
	FQuat targetRotation = ServerState.Transform.GetRotation();
	FQuat nextRotation = FQuat::Slerp(ClientStartTransform.GetRotation(), targetRotation, LerpRatio);

	if(MeshOffsetRoot != nullptr){
		MeshOffsetRoot->SetWorldRotation(nextRotation);
	}
	else {
		GetOwner()->SetActorRotation(nextRotation);
	}
}

float UShipMovementReplicator::VelocityToDerivative()
{
	 return ClientTimeBetweenLastUpdates * 100;  //Convert from meters per second to centimeters per second
}

bool UShipMovementReplicator::ShouldUpdateServer()
{
	if(GetOwnerRole() != ROLE_Authority) return false; //Only authority should send server updates

	APawn* Owner = Cast<APawn>(GetOwner()); // Need to get owner as pawn because GetOwner->GetRemoteRole() returns inconsistant results
	if(Owner == nullptr) return false;

	APlayerController* playerController = Cast<APlayerController>(Owner->GetController());
	bool isPlayerControlled = playerController != nullptr;
	if (isPlayerControlled) {
		return Owner->IsLocallyControlled(); //Check if we are server as player
	}
	else {
		return true; //If no player is controlling, the ship should move on it's own
	}
}

