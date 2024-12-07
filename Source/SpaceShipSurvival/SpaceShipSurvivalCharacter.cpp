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
#include "GameFramework/CharacterMovementComponent.h"

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

void ASpaceShipSurvivalCharacter::WalkingLook(const FVector2D LookInput)
{
	FRotator CameraRotation = GetFirstPersonCameraComponent()->GetRelativeRotation();
	FRotator LookRotation = FRotator(-LookInput.Y, 0, 0);
	bool LookingUp = CameraRotation.Pitch >= 85 && LookInput.Y < 0.f;
	bool LookingDown = CameraRotation.Pitch <= -85 && LookInput.Y > 0.f;
	if (LookingUp || LookingDown)
	{
		GetFirstPersonCameraComponent()->AddLocalRotation(LookRotation);
	}

	FRotator SpinRotation = FRotator(0, LookInput.X, 0);
	AddActorLocalRotation(SpinRotation);
}

void ASpaceShipSurvivalCharacter::FlyingLook(const FVector2D LookInput)
{
	// add yaw and pitch input
	//Rotate camera until 90 degrees then rotate capsule
	FRotator CameraRotation = GetFirstPersonCameraComponent()->GetRelativeRotation();
	FRotator LookRotation = FRotator(-LookInput.Y, 0, 0);
	bool LookingUp = CameraRotation.Pitch >= 85 && -LookInput.Y > 0.f;
	bool LookingDown = CameraRotation.Pitch <= -85 && -LookInput.Y < 0.f;
	UE_LOG(LogTemp, Warning, TEXT("Camera Pitch: %f"), CameraRotation.Pitch);
	if (LookingUp || LookingDown)
	{
		AddActorLocalRotation(LookRotation);
	}
	else
	{
		GetFirstPersonCameraComponent()->AddLocalRotation(LookRotation);
	}
	FRotator SpinRotation = FRotator(0, LookInput.X, 0);
	AddActorLocalRotation(SpinRotation);
}

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

		//Rotating
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ASpaceShipSurvivalCharacter::Rotate);

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
	FVector ForwardDirection = FVector::ZeroVector;
	FVector RightDirection = FVector::ZeroVector;
	switch (GetCharacterMovement()->MovementMode)
	{
	case EMovementMode::MOVE_Walking :
		ForwardDirection = GetActorForwardVector();
		RightDirection = GetActorRightVector();
		break;
	case EMovementMode::MOVE_Flying :
		ForwardDirection = GetFirstPersonCameraComponent()->GetForwardVector();
		RightDirection = GetFirstPersonCameraComponent()->GetRightVector();
		break;
	default:
			break;
	}

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASpaceShipSurvivalCharacter::Rotate(const FInputActionValue& Value)
{
	float RotateSpeed = Value.Get<float>();
	if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Walking)
	{
		FRotator Rotation = FRotator(0, 0, RotateSpeed);
		AddActorLocalRotation(Rotation);
	}
}

void ASpaceShipSurvivalCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	EMovementMode MovementMode = GetCharacterMovement()->MovementMode;
	switch (MovementMode)
	{
		case EMovementMode::MOVE_Walking:
			WalkingLook(LookAxisVector);
			break;

		case MOVE_Flying:
			FlyingLook(LookAxisVector);
			break;
		default:
			break;
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
	auto PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		EnableInput(PlayerController);
	}
	Restart();
}

void ASpaceShipSurvivalCharacter::UnPossessed()
{
	Super::UnPossessed();
	auto PlayerController = Cast<APlayerController>(GetController());
	if(PlayerController == nullptr) return;
	DisableInput(PlayerController);
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
	GetCharacterMovement()->DisableMovement();
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
