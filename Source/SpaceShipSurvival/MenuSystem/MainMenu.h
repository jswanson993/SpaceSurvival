// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerDetails {
	GENERATED_BODY()

	FString ServerName;
	FString ServerPassword;
	FString ServerType;
	FString Players;
};

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UMainMenu(const FObjectInitializer &ObjectInitializer);
	virtual bool Initialize() override;

	void SetServerList(TArray<FServerDetails> Servers);

public:
	//Menu Components
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;
	//Main Menu Components
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;
	UPROPERTY(meta = (BindWidget))
	class UButton* HostGameButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinGameButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* OptionsButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;
	//Host Menu Components
	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UEditableTextBox* ServerNameTextBox;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UEditableTextBox* PasswordTextBox;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UComboBoxString* ServerTypeDropdown;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USlider* PlayerLimitSlider;
	UPROPERTY(meta = (BindWidget))
	class UButton* StartGameButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;
	//Join Menu Components
	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ServerList;
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton2;

	void SetMenuSystem(IMenuSystem* MenuSystem);
	void Setup();
	void TearDown();

private:

	IMenuSystem* _MenuSystem;

	UFUNCTION()
	void OnHostGameClicked();
	UFUNCTION()
	void OnJoinGameClicked();
	UFUNCTION()
	void OnOptionsClicked();
	UFUNCTION()
	void OnQuitClicked();
	UFUNCTION()
	void OnStartGameClicked();
	UFUNCTION()
	void OnBackClicked();
	UFUNCTION()
	void OnJoinClicked();

	TArray<FServerDetails> FoundServers;
	TSubclassOf<class UServerLine> ServerLineClass;
};
