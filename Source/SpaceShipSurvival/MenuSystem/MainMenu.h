// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerDetails {
	GENERATED_BODY()

	FString OwningUser;
	FString ServerName;
	FString ServerPassword;
	FString ServerType;
	FString Players;
};

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

public:
	UMainMenu(const FObjectInitializer &ObjectInitializer);
	virtual bool Initialize() override;

	void SetServerList(TArray<FServerDetails> Servers);
	void UpdateSelection(int32 Index);
	void ClearServerList();

public:
	//Menu Components
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;
	
	//Main Menu Components
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* HostGameButton;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* JoinGameButton;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* OptionsButton;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* QuitButton;
	
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
	class UCustomButton* StartGameButton;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* BackButton;
	
	//Join Menu Components
	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ServerList;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* JoinButton;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* BackButton2;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SearchingText;
	UPROPERTY(meta = (BindWidget))
	class UOverlay* PasswordOverlay;
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* AuthorizationTextbox;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* ConfirmButton;
	UPROPERTY(meta = (BindWidget))
	class UCustomButton* CancelButton;
	UPROPERTY(meta = (BindWidget))
	class UCheckBox* FriendsOnlyCheckBox;
	
	//Settings Menu Components
	UPROPERTY(meta = (BindWidget))
	class USettingsMenu* SettingsMenu;

private:

	TArray<FServerDetails> FoundServers;
	TSubclassOf<class UUserWidget> ServerLineClass;
	int32 SelectedIndex;

	UFUNCTION()
	void OnHostGameClicked();
	UFUNCTION()
	void OnJoinGameClicked();
	UFUNCTION()
	void OnOptionsClicked();
	UFUNCTION()
	void OnStartGameClicked();
	UFUNCTION()
	void OnBackClicked();
	UFUNCTION()
	void OnJoinClicked();
	UFUNCTION()
	void OnConfirmClicked();
	UFUNCTION()
	void OnCancelClicked();
	UFUNCTION()
	void OnCheckBoxChanged(bool bIsChecked);
};
