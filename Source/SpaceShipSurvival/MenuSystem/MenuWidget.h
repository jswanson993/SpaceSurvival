// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetMenuSystem(IMenuSystem* MenuSystem);
	void Setup();
	UFUNCTION()
	void TearDown();

	UFUNCTION()
	void OnQuitClicked();

protected:
	IMenuSystem* _MenuSystem;
};
