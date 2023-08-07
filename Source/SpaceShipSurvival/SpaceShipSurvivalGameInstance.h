// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuSystem.h"
#include "SpaceShipSurvivalGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API USpaceShipSurvivalGameInstance : public UGameInstance, public IMenuSystem
{
	GENERATED_BODY()

public:
	USpaceShipSurvivalGameInstance(const FObjectInitializer& ObjectInitializer);

	void Init() override;

	void HostGame(FString ServerName, FString Password, FString ServerType, int32 PlayerLimit) override;

	void JoinGame() override;

	UFUNCTION(BlueprintCallable)
	void LoadMenu();

private:
	TSubclassOf<class UMainMenu> MainMenuClass;
	class UMainMenu* MainMenu;
	
};
