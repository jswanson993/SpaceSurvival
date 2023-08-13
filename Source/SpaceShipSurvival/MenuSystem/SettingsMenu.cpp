// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenu.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "UnrealClient.h"
#include "Engine.h"
#include "Kismet/KismetSystemLibrary.h"

bool USettingsMenu::Initialize()
{
	bool success = Super::Initialize();
	if(!success) return false;

	if(!ensure(ApplyButton != nullptr)) return false;
	ApplyButton->OnClicked.AddDynamic(this, &USettingsMenu::OnApplyClicked);

	
	return true;
}

void USettingsMenu::NativeOnInitialized()
{
	SetupResolution();
	SetupFullscreen();
}

void USettingsMenu::OnApplyClicked()
{
	FString fullscreenMode = FullscreenComboBox->GetSelectedOption();
	FString resolutionStr = ResolutionComboBox->GetSelectedOption();
	UGameUserSettings* userSettings = GEngine->GetGameUserSettings();
	if (fullscreenMode.Equals("Fullscreen")) {
		userSettings->SetFullscreenMode(EWindowMode::Fullscreen);
	}
	else if (fullscreenMode.Equals("Windowed Fullscreen")) {
		userSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
	}
	else {
		userSettings->SetFullscreenMode(EWindowMode::Windowed);
	}
	if(!fullscreenMode.Equals("Windowed")){
		FIntPoint* newRes = ResolutionMapping.Find(resolutionStr);
		if(newRes != nullptr){
			userSettings->SetScreenResolution(*newRes);
		}
	}
	userSettings->ApplySettings(false);
}

void USettingsMenu::SetupResolution()
{
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
	FVector2D currentResolution;

	if (!ensure(ResolutionComboBox != nullptr)) return;
	for (auto resolution : Resolutions) {
		FString res = FString::Printf(TEXT("%d / %d"), resolution.X, resolution.Y);
		ResolutionComboBox->AddOption(res);
		ResolutionMapping.Add(res, resolution);
	}
	UE_LOG(LogTemp, Warning, TEXT("Current resolution: %d / %d"), GSystemResolution.ResX, GSystemResolution.ResY);
	FString currentRes = FString::Printf(TEXT("%d / %d"), GSystemResolution.ResX, GSystemResolution.ResY);
	ResolutionComboBox->SetSelectedOption(currentRes);
}

void USettingsMenu::SetupFullscreen()
{
	EWindowMode::Type fullscreenMode = GEngine->GetGameUserSettings()->GetFullscreenMode();
	switch (fullscreenMode) {
		case EWindowMode::Fullscreen : FullscreenComboBox->SetSelectedOption("Fullscreen");
			break;
		case EWindowMode::WindowedFullscreen : FullscreenComboBox->SetSelectedOption("Windowed Fullscreen");
			break;
		case EWindowMode::Windowed : FullscreenComboBox->SetSelectedOption("Windowed");
			break;
		default:
			break;
	}
}
