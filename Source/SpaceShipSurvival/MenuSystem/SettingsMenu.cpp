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
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
	if (GEngine != nullptr) {
		CurrentResolution = GEngine->GameViewport->Viewport->GetSizeXY();
		UE_LOG(LogTemp, Warning, TEXT("Current Resolution: %s"), *CurrentResolution.ToString())
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Could not access GEngine"));
	}

	if (!ensure(ResolutionComboBox != nullptr)) return;
	for (auto resolution : Resolutions) {
		FString res = FString::Printf(TEXT("%d / %d"), resolution.X, resolution.Y);
		ResolutionComboBox->AddOption(res);
		ResolutionMapping.Add(res, resolution);
	}

	if(ResolutionMapping.FindKey(CurrentResolution) != nullptr){
		FString currentRes = *ResolutionMapping.FindKey(CurrentResolution);
		ResolutionComboBox->SetSelectedOption(currentRes);
	}
}

void USettingsMenu::OnApplyClicked()
{
	return;
}
