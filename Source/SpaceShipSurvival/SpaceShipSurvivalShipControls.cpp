// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipSurvivalShipControls.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"

#include "Ship/SpaceShipSurvivalShip.h"
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
		controller->Server_Possess(Ship);
	}
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

