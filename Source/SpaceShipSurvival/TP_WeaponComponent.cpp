// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "SpaceShipSurvivalCharacter.h"
#include "SpaceShipSurvivalProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

FString GetRoleStr(ENetRole Role) {

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


void UTP_WeaponComponent::Server_AttachWeapon_Implementation(ASpaceShipSurvivalCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	UE_LOG(LogTemp, Warning, TEXT("Owner: %s. Role: %s"), *GetOwner()->GetActorNameOrLabel(), *GetRoleStr(GetOwner()->GetLocalRole()));
	GetOwner()->SetOwner(Character);
	UE_LOG(LogTemp, Warning, TEXT("Setting Owner's Owner To Character With Role: %s"), *GetRoleStr(Character->GetLocalRole()))
		GetOwner()->SetRole(Character->GetLocalRole());
	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	SetIsReplicated(true);

	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);


	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}
	//Server_AttachWeapon(TargetCharacter);
	//return true;
}


bool UTP_WeaponComponent::Server_AttachWeapon_Validate(ASpaceShipSurvivalCharacter* TargetCharacter)
{
	
	//if (Character == nullptr)
	//{
	//	return false;
	//}

	//if (Character->GetHasRifle()) {
	//	return false;
	//}

	return true;
}



void UTP_WeaponComponent::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Client Fire"));
	UE_LOG(LogTemp, Warning, TEXT("Owning Character: %s  Role: %s"), *GetOwner()->GetOwner()->GetActorNameOrLabel(), *GetRoleStr(GetOwner()->GetLocalRole()));
	//If Player is Server only use server fire

		if (Character == nullptr || Character->GetController() == nullptr)
		{
			return;
		}
		if(GetOwner()->HasAuthority()){
			SpawnProjectile();
		}
	
		// Try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
		}
		if(Character->IsLocallyControlled()){
			PlayFireAnimation();
		}


}

void UTP_WeaponComponent::Server_Fire_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Server Fire"))
		Server_Fire();
		/*
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	SpawnProjectile();

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	PlayFireAnimation();

	*/
}

bool UTP_WeaponComponent::Server_Fire_Validate() {
	return true;
}

bool UTP_WeaponComponent::AttachWeapon(ASpaceShipSurvivalCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return false;
	}

	if (Character->GetHasRifle()) {
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Owner: %s. Role: %s"), *GetOwner()->GetActorNameOrLabel(),  *GetRoleStr(GetOwner()->GetLocalRole()));
	GetOwner()->SetOwner(Character);
	UE_LOG(LogTemp, Warning, TEXT("Setting Owner's Owner To Character With Role: %s"), *GetRoleStr(Character->GetLocalRole()))
	GetOwner()->SetRole(Character->GetLocalRole());
	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	SetIsReplicated(true);

		// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);


	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Server_Fire);
		}
	}
	//Server_AttachWeapon(TargetCharacter);
	return true;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void UTP_WeaponComponent::PlayFireAnimation()
{
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::SpawnProjectile()
{
	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			ActorSpawnParams.Owner = Character;
			ActorSpawnParams.Instigator = Character;

			// Spawn the projectile at the muzzle
			World->SpawnActor<ASpaceShipSurvivalProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
}
