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

}

void ASpaceShipSurvivalShipControls::BeginPlay()
{
	Super::BeginPlay();
}

void ASpaceShipSurvivalShipControls::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASpaceShipSurvivalShipControls, bIsBeingUsed);
}

void ASpaceShipSurvivalShipControls::Interact(APlayerController* PlayerController)
{
	Super::Interact(PlayerController);
	UE_LOG(LogTemp, Warning, TEXT("Called from here"));
	if(Ship == nullptr) return;

	if(bIsBeingUsed == false){
		bIsBeingUsed = true;
		PlayerController->Possess(Ship);
	}
}

