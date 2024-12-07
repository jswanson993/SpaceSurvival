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
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	
	SetNetUpdateFrequency(1);
	//NetUpdateFrequency = 15;
	//if (HasAuthority()) {
	//	NetUpdateFrequency = 15;
	//}
	
}

void ASpaceShipSurvivalShip::PossessedBy(AController* NewController)
{
	//Intentionally not calling Super::PossessedBy because it will trigger an unwanted network update
	//Copying the rest of PossessedBy implementation instead
	SetOwner(NewController);

	AController* const OldController = Controller;

	Controller = NewController;

#if UE_WITH_IRIS
	// The owning connection depends on the Controller having the new value.
	UpdateOwningNetConnection();
#endif

	if (Controller->PlayerState != nullptr)
	{
		SetPlayerState(Controller->PlayerState);
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (GetNetMode() != NM_Standalone)
		{
			SetReplicates(true);
			SetAutonomousProxy(true);
		}
	}
	else
	{
		CopyRemoteRoleFrom(GetDefault<APawn>());
	}

	// dispatch Blueprint event if necessary
	if (OldController != NewController)
	{
		ReceivePossessed(Controller);

		NotifyControllerChanged();
	}
	//Reset pawn so input can be remapped
	Restart();
}

void ASpaceShipSurvivalShip::UnPossessed()
{
	//Intentionally not calling Super::UnPossessed because it will trigger an unwanted network update
	//Copying the rest of UnPossesed implementation instead
	AController* const OldController = Controller;

	SetPlayerState(nullptr);
	SetOwner(nullptr);
	Controller = nullptr;

#if UE_WITH_IRIS
	// The owning connection depends on the Controller having the new value.
	UpdateOwningNetConnection();
#endif

	// Unregister input component if we created one
	DestroyPlayerInputComponent();

	// dispatch Blueprint event if necessary
	if (OldController)
	{
		ReceiveUnpossessed(OldController);
	}

	NotifyControllerChanged();

	ConsumeMovementInputVector();
	
}

FString GetRoleString2(ENetRole Role) {

	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	case ROLE_MAX:
		return "Max";
	default:
		return "Error";
	}
}


// Called every frame
void ASpaceShipSurvivalShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetRoleString2(GetLocalRole()), this, FColor::White, DeltaTime);
	RootComponent->ComponentVelocity = MovementComponent->GetVelocity();
}

// Called to bind functionality to input
void ASpaceShipSurvivalShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalShip::ApplyThrottle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASpaceShipSurvivalShip::ThrottleComplete);
		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalShip::ApplyTurn);
		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalShip::ApplyYaw);
		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Completed, this, &ASpaceShipSurvivalShip::TurnComplete);
		EnhancedInputComponent->BindAction(YawAction, ETriggerEvent::Completed, this, &ASpaceShipSurvivalShip::YawComplete);
		EnhancedInputComponent->BindAction(ExitAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalShip::Exit);
	}
}

void ASpaceShipSurvivalShip::ApplyThrottle(const FInputActionValue& Value)
{
	if(MovementComponent == nullptr) return;

	MovementComponent->SetThrottle(Value.Get<FInputActionValue::Axis1D>());
}

void ASpaceShipSurvivalShip::ThrottleComplete(const FInputActionValue& Value)
{
	if(MovementComponent == nullptr) return;
	MovementComponent->SetThrottle(0);
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
		OnExitShip.Broadcast();

		ASpaceSurvivalCharacterController* characterController = Cast<ASpaceSurvivalCharacterController>(GetController());
		if (characterController != nullptr) {
			characterController->PossessDefaultPawn();
		}
	}

	Restart();
}

void ASpaceShipSurvivalShip::Restart()
{
	Super::Restart();

	APlayerController* controller = Cast<APlayerController>(GetController());
	if(controller == nullptr) return;
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(controller->GetLocalPlayer())) {
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(ShipMappingContext, 0);
	}
}
