// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipSurvivalShipControls.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Ship/SpaceShipSurvivalShip.h"
#include "SpaceSurvivalCharacterController.h"
#include "SpaceShipSurvivalCharacter.h"

ASpaceShipSurvivalShipControls::ASpaceShipSurvivalShipControls()
{
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	//Collider->SetupAttachment(RootComponent);
	RootComponent = Collider;
	Seat = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Seat"));
	Seat->SetupAttachment(Collider);

	bReplicates;
}

void ASpaceShipSurvivalShipControls::BeginPlay()
{
	Super::BeginPlay();
	
	Ship = Cast<ASpaceShipSurvivalShip>(this->GetParentActor());
}

void ASpaceShipSurvivalShipControls::Interact_Implementation(APlayerController* PlayerController)
{
	auto controller = Cast<ASpaceSurvivalCharacterController>(PlayerController);
	if(controller == nullptr) return;
	if(Ship == nullptr) return;
	if (CheckIfBeingUsed() == false) {
		if (controller->IsLocalController()) {
			PromptWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		Ship->OnExitShip.AddDynamic(this, &ASpaceShipSurvivalShipControls::OnExitShip);
		AttachPawnToControls(controller->GetPawn());
		controller->Server_Possess(Ship);
	}
}

void ASpaceShipSurvivalShipControls::AttachPawnToControls(APawn* PawnToAttach)
{
	ASpaceShipSurvivalCharacter* playerCharacter = Cast<ASpaceShipSurvivalCharacter>(PawnToAttach);
	playerCharacter->GetInSeat(this);
}

bool ASpaceShipSurvivalShipControls::CheckIfBeingUsed()
{
	if(Ship == nullptr) return true;
	APlayerController* playerController = Cast<APlayerController>(Ship->GetController());

	//If the controller on the ship is a player controller then it is in use
	if (playerController != nullptr) {
		return true;
	}
	else {
		return false;
	}
}

void ASpaceShipSurvivalShipControls::OnExitShip()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	if (AttachedActors.Num() > 1) {
		UE_LOG(LogTemp, Warning, TEXT("Ship Controls should only have 1 attached actor. Found %d"), AttachedActors.Num());
	}

	if (AttachedActors.Num() == 1) {

		FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules::KeepWorldTransform;
		//DetachmentRules.RotationRule = EDetachmentRule::KeepRelative;
		ACharacter* PlayerCharacter = Cast<ACharacter>(AttachedActors[0]);
		
		
		UE_LOG(LogTemp, Warning, TEXT("Detatching"));
		PlayerCharacter->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;
		PlayerCharacter->DetachFromActor(DetachmentRules);
		if (GetAttachParentActor() != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *GetAttachParentActor()->GetVelocity().ToString());
			//PlayerCharacter->GetCharacterMovement()->Velocity = GetAttachParentActor()->GetVelocity();
		}
		
	}

	if (PromptWidget != nullptr && PromptWidget->IsInViewport()) {
		PromptWidget->SetVisibility(ESlateVisibility::Visible);
	}
	if(Ship != nullptr){
		Ship->OnExitShip.RemoveAll(this);
	}
}

void ASpaceShipSurvivalShipControls::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASpaceShipSurvivalShipControls, Ship);
}

