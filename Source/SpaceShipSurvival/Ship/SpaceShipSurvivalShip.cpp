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

#include "ShipMovementComponent.h"
#include "ShipMovementReplicator.h"
#include "SpaceShipSurvival/SpaceSurvivalCharacterController.h"
#include "SpaceShipSurvival/SpaceShipSurvivalShipControls.h"

// Sets default values
ASpaceShipSurvivalShip::ASpaceShipSurvivalShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	RootComponent = BoxCollider;

	MeshOffsetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MeshOffsetRoot"));
	MeshOffsetRoot->SetupAttachment(RootComponent);

	HullMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hull"));
	HullMesh->SetupAttachment(MeshOffsetRoot);
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(MeshOffsetRoot);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	
	MovementComponent = CreateDefaultSubobject<UShipMovementComponent>(TEXT("Movement Component"));
	MovementReplicator = CreateDefaultSubobject<UShipMovementReplicator>(TEXT("Movement Replicator"));

	bReplicates = true;
	SetReplicateMovement(false);
	
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

	if (HasAuthority()) {
		NetUpdateFrequency = 10;
	}
	
}

void ASpaceShipSurvivalShip::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//Reset pawn so input can be remapped
	Restart();
}

// Called every frame
void ASpaceShipSurvivalShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
		EnhancedInputComponent->BindAction(ExitAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalShip::Exit);
		UE_LOG(LogTemp, Warning, TEXT("Bound Actions for Space Ship"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Not Using Enhanced Input"));
	}

}

void ASpaceShipSurvivalShip::ApplyThrottle(const FInputActionValue& Value)
{
	if(MovementComponent == nullptr) return;
	MovementComponent->SetThrottle(Value.Get<FInputActionValue::Axis1D>());
}

void ASpaceShipSurvivalShip::ApplyTurn(const FInputActionValue& Value)
{
	if(MovementComponent == nullptr) return;

	FVector2D turnAmount = Value.Get<FInputActionValue::Axis2D>();
	
	MovementComponent->SetRoll(turnAmount.X);
	MovementComponent->SetPitch(turnAmount.Y);
}

void ASpaceShipSurvivalShip::ApplyYaw(const FInputActionValue& Value)
{
	if(MovementComponent == nullptr) return;

	MovementComponent->SetYaw(Value.Get<FInputActionValue::Axis1D>());
	
}

void ASpaceShipSurvivalShip::TurnComplete(const FInputActionValue& Value)
{
	if(MovementComponent == nullptr) return;

	MovementComponent->SetPitch(0);
	MovementComponent->SetRoll(0);
}

void ASpaceShipSurvivalShip::YawComplete(const FInputActionValue& Value)
{
	if(MovementComponent == nullptr) return;

	MovementComponent->SetYaw(0);
}

void ASpaceShipSurvivalShip::Exit(const FInputActionValue& Value)
{
	bool exitPressed = Value.Get<bool>();
	if (exitPressed == true) {
		if (Controls != nullptr) {
			Controls->SetIsBeingUsed(false);
		}
		ASpaceSurvivalCharacterController* characterController = Cast<ASpaceSurvivalCharacterController>(GetController());
		if (characterController != nullptr) {
			characterController->PossessDefaultPawn();
		}
	}
}

void ASpaceShipSurvivalShip::Restart()
{
	Super::Restart();
	APlayerController* controller = Cast<APlayerController>(GetController());
	if(controller == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("Being Possessed"));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(controller->GetLocalPlayer())) {
		Subsystem->ClearAllMappings();
		UE_LOG(LogTemp, Warning, TEXT("Adding Mapping Context"));
		Subsystem->AddMappingContext(ShipMappingContext, 0);
	}
}
