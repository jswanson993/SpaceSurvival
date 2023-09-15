// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipSurvivalShipControls.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"

#include "SpaceSurvivalCharacterController.h"

ASpaceShipSurvivalShipControls::ASpaceShipSurvivalShipControls()
{
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetupAttachment(RootComponent);
	Seat = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Seat"));
	Seat->SetupAttachment(Collider);

	bReplicates;
}

void ASpaceShipSurvivalShipControls::BeginPlay()
{
	Super::BeginPlay();
	OnRep_SetShip(Ship);
}

void ASpaceShipSurvivalShipControls::Interact_Implementation(APlayerController* PlayerController)
{
	auto controller = Cast<ASpaceSurvivalCharacterController>(PlayerController);
	if(controller == nullptr) return;
	if (bIsBeingUsed == false) {
		bIsBeingUsed = true;
		if (controller->IsLocalController()) {
			PromptWidget->RemoveFromParent();
		}
		controller->Server_Possess(Ship);


	}
}

void ASpaceShipSurvivalShipControls::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASpaceShipSurvivalShipControls, bIsBeingUsed);
	DOREPLIFETIME(ASpaceShipSurvivalShipControls, Ship);
}

