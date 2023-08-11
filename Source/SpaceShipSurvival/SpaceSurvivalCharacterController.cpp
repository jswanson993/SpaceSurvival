// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceSurvivalCharacterController.h"
#include "SpaceShipSurvivalGameInstance.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"

ASpaceSurvivalCharacterController::ASpaceSurvivalCharacterController()
{
}

void ASpaceSurvivalCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

}

void ASpaceSurvivalCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();
	auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if(EnhancedInputComponent != nullptr){
		EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Started, this, &ASpaceSurvivalCharacterController::LoadMenu);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Could not setup enhanced input"));
	}
}

void ASpaceSurvivalCharacterController::LoadMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("Loading Menu"))
	auto gameInstance = Cast<USpaceShipSurvivalGameInstance>(GetGameInstance());
	gameInstance->LoadInGameMenu();
}
