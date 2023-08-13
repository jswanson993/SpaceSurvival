// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenu.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API USettingsMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* FullscreenComboBox;
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* ResolutionComboBox;
	UPROPERTY(meta = (BindWidget))
	class UButton* ApplyButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;
	virtual bool Initialize() override;
	virtual void NativeOnInitialized() override;

private:

	UFUNCTION()
	void OnApplyClicked();

	TArray<FIntPoint> Resolutions;
	TMap<FString, FIntPoint> ResolutionMapping;
	FIntPoint CurrentResolution;

	FViewport* ViewPort;
};
