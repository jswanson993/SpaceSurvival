// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpaceShipSurvivalCharacter.h"
#include "SpaceShipSurvivalProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "AIController.h"

#include "SpaceSurvivalCharacterController.h"

//////////////////////////////////////////////////////////////////////////
// ASpaceShipSurvivalCharacter

ASpaceShipSurvivalCharacter::ASpaceShipSurvivalCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	bReplicates = true;
}

void ASpaceShipSurvivalCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void ASpaceShipSurvivalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalCharacter::Move);


		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalCharacter::Look);

		//Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalCharacter::Interact);
	}
}


void ASpaceShipSurvivalCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ASpaceShipSurvivalCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASpaceShipSurvivalCharacter::Interact()
{

	APlayerController* playerController = Cast<APlayerController>(GetController());
	if(playerController == nullptr) return;
	
	OnInteract.Broadcast(playerController);
}

void ASpaceShipSurvivalCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	Restart();
}

void ASpaceShipSurvivalCharacter::UnPossessed()
{
	Super::UnPossessed();
}

void ASpaceShipSurvivalCharacter::Restart()
{
	Super::Restart();

	ASpaceSurvivalCharacterController* controller = Cast<ASpaceSurvivalCharacterController>(GetController());
	if (controller == nullptr) return;
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(controller->GetLocalPlayer())) {
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

FString GetRoleString(ENetRole Role) {
	
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

void ASpaceShipSurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugString(GetWorld(), FVector(0,0,100), GetRoleString(GetLocalRole()), this, FColor::White, DeltaTime);
}

void ASpaceShipSurvivalCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ASpaceShipSurvivalCharacter::GetHasRifle()
{
	return bHasRifle;
}

void ASpaceShipSurvivalCharacter::GetInSeat(AActor* OtherActor)
{ 
	Server_AttachToActor(OtherActor);
}

void ASpaceShipSurvivalCharacter::GetOutOfSeat()
{
	Server_DetachFromActor();
}

void ASpaceShipSurvivalCharacter::Server_AttachToActor_Implementation(AActor* OtherActor)
{
	FAttachmentTransformRules attachmentRules = FAttachmentTransformRules::KeepWorldTransform;
	AttachToActor(OtherActor, attachmentRules);
}

void ASpaceShipSurvivalCharacter::Server_DetachFromActor_Implementation()
{
	FDetachmentTransformRules detachmentRules = FDetachmentTransformRules::KeepWorldTransform;
	DetachFromActor(detachmentRules);

	if(OldParent != nullptr){
		FAttachmentTransformRules attachmentRules = FAttachmentTransformRules::KeepWorldTransform;
		AttachToActor(OldParent, attachmentRules);
	}
}
