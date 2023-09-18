// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "ShipMovementComponent.generated.h"

USTRUCT()
struct FShipMove {
	GENERATED_BODY()
	UPROPERTY()
	float Throttle;
	UPROPERTY()
	float Pitch;
	UPROPERTY()
	float Yaw;
	UPROPERTY()
	float Roll;
	UPROPERTY()
	float DeltaTime;
	UPROPERTY()
	float Time;

	bool IsValid() const
	{
		return FMath::Abs(Throttle) <= 1 && FMath::Abs(Yaw) <= 1;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACESHIPSURVIVAL_API UShipMovementComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	//Force Added when throttle is fully down (N)
	UPROPERTY(EditAnywhere, Category = Movement)
	float MaxThrusterForce = 10000;

	//Angular Force Added when fully rolling (N)
	UPROPERTY(EditAnywhere, Category = Movement)
	float MaxRollForce = 1000;


	float Throttle;
	UPROPERTY(EditAnywhere, Category = Movement)
	float MaxThrottle = 100;
	UPROPERTY(EditAnywhere, Category = Movement)
	float MinThrottle = -100;
	
	float Pitch;
	float Yaw;
	float Roll;

	FShipMove LastMove;
	
	UPROPERTY(EditAnywhere, Category = Movement)
	float MinPitchRadius = 50;
	UPROPERTY(EditAnywhere, Category = Movement)
	float MinYawRadius = 10;

	UPROPERTY(EditAnywhere, Category = Movement)
	float Mass = 1000;

	//How quickly fuel is being consumed (Liters/Minute)
	float FuelConsumption;
	UPROPERTY(EditAnywhere, Category = Fuel)
	float MaximumFuel;
	UPROPERTY(BlueprintReadOnly, Category = Fuel, meta = (AllowPrivateAccess = "true"))
	float FuelRemaining;
	//Cannot go faster than Speed of Light. (m/s)
	const float MaxVelocity = 299792458;

	FVector Velocity;

public:	
	// Sets default values for this component's properties
	UShipMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FVector GetVelocity() { return Velocity; }
	void SetVelocity(FVector NewVelocity) { Velocity = NewVelocity; }

	void SetThrottle(float ThrottleValue);
	void SetPitch(float PitchValue);
	void SetYaw(float YawValue);
	void SetRoll(float RollValue);

	void SimulateMove(FShipMove Move);

	void UpdateLocationFromVelocity(FShipMove& Move);


	FShipMove GetLastMove(){ return LastMove; }

private:
	void CalculateForwardVelocity(FShipMove Move);
	void CalculateAngularVelocity(FShipMove Move);
	
	FShipMove CreateMove(float DeltaTime);
	bool ShouldCreateNewMove();
};
