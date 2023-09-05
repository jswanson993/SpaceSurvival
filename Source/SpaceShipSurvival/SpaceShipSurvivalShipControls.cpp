// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipSurvivalShipControls.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

#include "Interactable/InteractableComponent.h"

ASpaceShipSurvivalShipControls::ASpaceShipSurvivalShipControls()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetupAttachment(RootComponent);
	Seat = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Seat"));
	Seat->SetupAttachment(Collider);
	Interactable = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interaction Trigger"));
	Interactable->SetupAttachment(RootComponent);
}

void ASpaceShipSurvivalShipControls::BeginPlay()
{
	Super::BeginPlay();
}

void ASpaceShipSurvivalShipControls::Interact_Implementation(APlayerController* PlayerController)
{

	UE_LOG(LogTemp, Warning, TEXT("Called from here"));
	if (Ship == nullptr) return;

	if (bIsBeingUsed == false) {
		bIsBeingUsed = true;
		PlayerController->Possess(Ship);
	}
}

void ASpaceShipSurvivalShipControls::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASpaceShipSurvivalShipControls, bIsBeingUsed);
}

