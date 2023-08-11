// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpaceShipSurvivalGameMode.h"
#include "SpaceShipSurvivalCharacter.h"
#include "SpaceSurvivalCharacterController.h"
#include "UObject/ConstructorHelpers.h"

ASpaceShipSurvivalGameMode::ASpaceShipSurvivalGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(TEXT("/Script/SpaceSurvivalCharacterController"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	PlayerControllerClass = ASpaceSurvivalCharacterController::StaticClass();
}
