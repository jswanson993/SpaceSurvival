// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipSurvivalShip.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

// Sets default values
ASpaceShipSurvivalShip::ASpaceShipSurvivalShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	RootComponent = BoxCollider;
	//BoxCollider->SetupAttachment(RootComponent);
	HullMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hull"));
	HullMesh->SetupAttachment(BoxCollider);
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ASpaceShipSurvivalShip::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(ShipMappingContext, 0);
		}
	}
	
}

void ASpaceShipSurvivalShip::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APlayerController* playerController = Cast<APlayerController>(NewController);
	if(playerController == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("Being Possessed"));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer())) {
		UE_LOG(LogTemp, Warning, TEXT("Adding Mapping Context"));
		Subsystem->AddMappingContext(ShipMappingContext, 0);
	}
}

// Called every frame
void ASpaceShipSurvivalShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateForwardVelocity(DeltaTime);
	CalculateAngularVelocity(DeltaTime);

	FVector Translation = Velocity * DeltaTime * 100;
	FHitResult CollisionResult;
	AddActorWorldOffset(Translation, true, &CollisionResult);

	if (CollisionResult.IsValidBlockingHit()) {
		UE_LOG(LogTemp, Warning, TEXT("Blocking Hit"));
		Velocity = FVector::Zero();
	}

}

// Called to bind functionality to input
void ASpaceShipSurvivalShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("Using Enhanced Input"));
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalShip::ApplyThrottle);
		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalShip::ApplyTurn);
		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalShip::ApplyYaw);
		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Completed, this, &ASpaceShipSurvivalShip::TurnComplete);
		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Completed, this, &ASpaceShipSurvivalShip::YawComplete);
		UE_LOG(LogTemp, Warning, TEXT("Bound Actions for Space Ship"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Not Using Enhanced Input"));
	}

}

void ASpaceShipSurvivalShip::ApplyThrottle(const FInputActionValue& Value)
{
	float desiredThrottle = Throttle;
	desiredThrottle += Value.Get<FInputActionValue::Axis1D>();

	Throttle = FMath::Clamp(desiredThrottle, MinThrottle, MaxThrottle);
	UE_LOG(LogTemp, Warning, TEXT("Applying %f amount of throttle"), Throttle);
}

void ASpaceShipSurvivalShip::ApplyTurn(const FInputActionValue& Value)
{
	FVector2D turnAmount = Value.Get<FInputActionValue::Axis2D>();
	Roll = turnAmount.X;
	Pitch = turnAmount.Y;
	UE_LOG(LogTemp, Warning, TEXT("Applying %f amount of roll"), Roll);
	UE_LOG(LogTemp, Warning, TEXT("Applying %f amount for pitch"), Pitch);
}

void ASpaceShipSurvivalShip::ApplyYaw(const FInputActionValue& Value)
{
	Yaw = Value.Get<FInputActionValue::Axis1D>();
	UE_LOG(LogTemp, Warning, TEXT("Applying %f amount of yaw"), Yaw);
}

void ASpaceShipSurvivalShip::CalculateForwardVelocity(float DeltaTime)
{
	FVector force = GetActorForwardVector() * MaxThrusterForce * Throttle;
	FVector acceleration = force / Mass;
	Velocity += acceleration * DeltaTime;
}

void ASpaceShipSurvivalShip::CalculateAngularVelocity(float DeltaTime)
{

/**
	float DeltaLocation = Velocity.Dot(GetOwner()->GetActorForwardVector()) * DeltaTime;
	float RotationAngle = DeltaLocation / MinimumTurningRadius * _SteeringThrow;
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);

	Velocity = RotationDelta.RotateVector(Velocity);
	GetOwner()->AddActorWorldRotation(RotationDelta);
	**/


	
	float rollForce = MaxRollForce * Roll * DeltaTime;
	float rollAcceleration = rollForce / Mass;
	FQuat RollDelta(GetActorForwardVector(), rollAcceleration);
	AddActorWorldRotation(RollDelta);

	float DeltaLocation = Velocity.Dot(GetActorForwardVector()) * DeltaTime;
	float PitchAngle = DeltaLocation / MinPitchRadius * Pitch;
	FQuat PitchDelta(GetActorRightVector(), PitchAngle);
	Velocity = PitchDelta.RotateVector(Velocity);
	AddActorWorldRotation(PitchDelta);

	float YawAngle = DeltaLocation / MinYawRadius * Yaw;
	FQuat YawDelta(GetActorUpVector(), YawAngle);
	Velocity = YawDelta.RotateVector(Velocity);
	AddActorWorldRotation(YawDelta);


	//FQuat RotationDelta;
	//RotationDelta = PitchDelta + YawDelta;
	//Velocity = RotationDelta.RotateVector(Velocity);
	//RotationDelta += RollDelta;
	//AddActorWorldRotation(RotationDelta);
}

