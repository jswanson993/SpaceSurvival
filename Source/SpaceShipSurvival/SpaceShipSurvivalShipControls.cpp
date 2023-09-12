// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipSurvivalShipControls.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

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
	if (bIsBeingUsed == false) {
		bIsBeingUsed = true;
		switch (PlayerController->GetLocalRole())
		{
		case ROLE_None:
			break;
		case ROLE_SimulatedProxy:
			break;
		case ROLE_AutonomousProxy:
			break;
		case ROLE_Authority:
			Server_PossessShip(PlayerController);
		case ROLE_MAX:
			break;
		default:
			break;
		}
		Server_PossessShip(PlayerController);
	}
}

void ASpaceShipSurvivalShipControls::Server_PossessShip_Implementation(APlayerController* PlayerController)
{
	UE_LOG(LogTemp, Warning, TEXT("Possessing Ship"))
	if(Ship == nullptr) return;

	if (PlayerController->IsLocalController()) {
		PlayerController->Possess(Ship);
	}

}

void ASpaceShipSurvivalShipControls::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASpaceShipSurvivalShipControls, bIsBeingUsed);
	DOREPLIFETIME(ASpaceShipSurvivalShipControls, Ship);
}

