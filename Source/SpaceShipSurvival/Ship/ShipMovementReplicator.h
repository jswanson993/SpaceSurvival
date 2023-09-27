// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShipMovementComponent.h"
#include "ShipMovementReplicator.generated.h"

USTRUCT()
struct FShipState {
	GENERATED_BODY()
	UPROPERTY()
	FShipMove LastMove;
	UPROPERTY()
	FVector Velocity;
	UPROPERTY()
	FTransform Transform;
};


struct FHermiteCubicSpline {
	FVector StartLocation, StartDerivative, TargetLocation, TargetDerivative;
	FVector InterpolateLocation(float LerpRatio) {
		return FMath::CubicInterp(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio);
	}
	FVector InterpolateDerivative(float LerpRatio) {
		return FMath::CubicInterpDerivative(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio);
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACESHIPSURVIVAL_API UShipMovementReplicator : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FShipState ServerState;

	TArray<FShipMove> UnacknowlegedMoves;
	UPROPERTY()
	class UShipMovementComponent* MovementComponent;
	FShipMove CurrentMove;

	FTransform ClientStartTransform;
	FVector ClientStartVelocity;

	float ClientSimulatedTime = 0.f;
	float ClientTimeSinceLastUpdate = 0.f;
	float ClientTimeBetweenLastUpdates = 0.f;
	UPROPERTY()
	USceneComponent* MeshOffsetRoot;

public:	
	// Sets default values for this component's properties
	UShipMovementReplicator();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetMeshOffsetRoot(USceneComponent* Root) {MeshOffsetRoot = Root;}
	UFUNCTION(NetMulticast, Reliable)
	void Client_ForceUpdate();
private:
	
	void ClientTick(float DeltaTime);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FShipMove Move);
	void UpdateServerState(const FShipMove& Move);
	void ClearAcknowledgedMoves(FShipMove LastMove);
	
	UFUNCTION()
	void OnRep_ServerState();
	UFUNCTION()
	void AutonomousProxy_OnRep_ServerState();
	void SimulatedProxy_OnRep_ServerState();
	FHermiteCubicSpline CreateSpline();
	void InterpolateLocation(FHermiteCubicSpline &Spline, float LerpRatio);
	void InterpolateVelocity(FHermiteCubicSpline &Spline, float LerpRatio);
	void InterpolateRotation(float LerpRatio);
	float VelocityToDerivative();

	bool ShouldUpdateServer();
};
