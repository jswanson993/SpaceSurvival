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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	USceneComponent* MeshOffsetRoot;

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
	virtual void Restart() override;

private:
};
