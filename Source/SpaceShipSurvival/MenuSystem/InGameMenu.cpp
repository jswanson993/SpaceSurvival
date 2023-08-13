// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Overlay.h"

#include "CustomButton.h"
#include "SettingsMenu.h"

bool UInGameMenu::Initialize()
{
	bool success = Super::Initialize();
	if(success == false) return false;
	
	if(!ensure(ResumeButton != nullptr)) return false;
	ResumeButton->CustomButton->OnClicked.AddDynamic(this, &UMenuWidget::TearDown);
	if (!ensure(SettingsButton != nullptr)) return false;
	SettingsButton->CustomButton->OnClicked.AddDynamic(this, &UInGameMenu::OnClickSettings);
	if (!ensure(QuitToMenuButton != nullptr)) return false;
	QuitToMenuButton->CustomButton->OnClicked.AddDynamic(this, &UInGameMenu::OnClickQuitToMenu);
	if (!ensure(QuitToDesktopButton != nullptr)) return false;
	QuitToDesktopButton->CustomButton->OnClicked.AddDynamic(this, &UMenuWidget::OnQuitClicked);
	if(!ensure(SettingsMenu != nullptr)) return false;
	SettingsMenu->BackButton->OnClicked.AddDynamic(this, &UInGameMenu::OnBackClicked);
	return true;
}

void UInGameMenu::OnClickSettings()
{
	if(!ensure(MenuSwitcher != nullptr)) return;
	if(!ensure(SettingsMenu != nullptr)) return;

	MenuSwitcher->SetActiveWidget(SettingsMenu);
}

void UInGameMenu::OnClickQuitToMenu()
{
	if (_MenuSystem != nullptr) {
		TearDown();
		_MenuSystem->LeaveGame();
	}
}

void UInGameMenu::OnBackClicked()
{
	if(!ensure(MenuSwitcher != nullptr)) return;
	if(!ensure(InGameMenu != nullptr)) return;

	MenuSwitcher->SetActiveWidget(InGameMenu);

}
