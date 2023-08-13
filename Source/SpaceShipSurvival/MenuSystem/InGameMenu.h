// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.h"
#include "InGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API UInGameMenu : public UMenuWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* ResumeButton;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* SettingsButton;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* QuitToMenuButton;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* QuitToDesktopButton;
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;
	UPROPERTY(meta = (BindWidget))
	class USettingsMenu* SettingsMenu;
	UPROPERTY(meta = (BindWidget))
	class UOverlay* InGameMenu;

	bool Initialize();

private:
	UFUNCTION()
	void OnClickSettings();
	UFUNCTION()
	void OnClickQuitToMenu();
	UFUNCTION()
	void OnBackClicked();

};
