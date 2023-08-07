// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipSurvivalGameInstance.h"
#include "MenuSystem/MainMenu.h"
#include "UObject/ConstructorHelpers.h"

//TODO Setup Online Subsystem

USpaceShipSurvivalGameInstance::USpaceShipSurvivalGameInstance(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UMainMenu> MainMenuClassFinder(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (MainMenuClassFinder.Class != nullptr) {
		MainMenuClass = MainMenuClassFinder.Class;
	}
}

void USpaceShipSurvivalGameInstance::Init()
{
	Super::Init();

}

void USpaceShipSurvivalGameInstance::HostGame(FString ServerName, FString Password, FString ServerType, int32 PlayerLimit)
{
	if(MainMenu != nullptr){
		MainMenu->TearDown();
	}
}

void USpaceShipSurvivalGameInstance::JoinGame()
{
	if (MainMenu != nullptr) {
		MainMenu->TearDown();
	}
}

void USpaceShipSurvivalGameInstance::LoadMenu()
{
	if(!ensure(MainMenuClass != nullptr)) return;
	MainMenu = CreateWidget<UMainMenu>(this, MainMenuClass);
	if(!ensure(MainMenu != nullptr)) return;
	MainMenu->SetMenuSystem(this);
	
	MainMenu->Setup();
}
