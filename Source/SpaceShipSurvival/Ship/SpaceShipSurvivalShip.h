// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "SpaceShipSurvivalShip.generated.h"

UCLASS()
class SPACESHIPSURVIVAL_API ASpaceShipSurvivalShip : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category=Collision)
	class UBoxComponent* BoxCollider;

	UPROPERTY(VisibleAnywhere, Category=Mesh)
	UStaticMeshComponent* HullMesh;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	class UCameraComponent* Camera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* ShipMappingContext;

	/** Throttle Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ThrottleAction;

	/** Pitch/Yaw Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* TurnAction;

	/** Roll Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* YawAction;

	//Force Added when throttle is fully down (N)
	UPROPERTY(EditAnywhere, Category = Movement)
	float MaxThrusterForce = 1000;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UShipMovementComponent* MovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UShipMovementReplicator* MovementReplicator;


public:
	// Sets default values for this pawn's properties
	ASpaceShipSurvivalShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ApplyThrottle(const FInputActionValue& Value);
	void ApplyTurn(const FInputActionValue& Value);
	void ApplyYaw(const FInputActionValue& Value);
	void TurnComplete(const FInputActionValue& Value);
	void YawComplete(const FInputActionValue& Value);

private:

	void CalculateForwardVelocity(float DeltaTime);
	void CalculateAngularVelocity(float DeltaTime);

};
