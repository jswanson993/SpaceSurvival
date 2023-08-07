// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"


//TODO Add functionality to hosting and joining games

bool UMainMenu::Initialize()
{
    bool success = Super::Initialize();
    if(!success) return false;

    //Bind On Click Events
    if(!ensure(HostGameButton != nullptr)) return false;
    HostGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnHostGameClicked);
    if (!ensure(JoinGameButton != nullptr)) return false;
    JoinGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinGameClicked);
    if (!ensure(OptionsButton != nullptr)) return false;
    OptionsButton->OnClicked.AddDynamic(this, &UMainMenu::OnOptionsClicked);
    if (!ensure(QuitButton != nullptr)) return false;
    QuitButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitClicked);
    if (!ensure(StartGameButton != nullptr)) return false;
    StartGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnStartGameClicked);
    if (!ensure(BackButton != nullptr)) return false;
    BackButton->OnClicked.AddDynamic(this, &UMainMenu::OnBackClicked);
    if (!ensure(JoinButton != nullptr)) return false;
    JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinClicked);
    if (!ensure(BackButton2 != nullptr)) return false;
    BackButton2->OnClicked.AddDynamic(this, &UMainMenu::OnBackClicked);

    return true;
}

void UMainMenu::OnHostGameClicked()
{
    if (MenuSwitcher != nullptr && HostMenu != nullptr){
        MenuSwitcher->SetActiveWidget(HostMenu);
    }
}

void UMainMenu::OnJoinGameClicked()
{
    if (MenuSwitcher != nullptr && JoinMenu != nullptr) {
        MenuSwitcher->SetActiveWidget(JoinMenu);
    }
}

void UMainMenu::OnOptionsClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Clicked Options"));
}

void UMainMenu::OnQuitClicked()
{
    UWorld* world = GetWorld();
    if(!ensure(world != nullptr)) return;

    APlayerController* playerController = world->GetFirstPlayerController();
    if(!ensure(playerController != nullptr)) return;
    playerController->ConsoleCommand("Exit");

}

void UMainMenu::OnStartGameClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Clicked Start Game"));
}

void UMainMenu::OnBackClicked()
{
    if (MenuSwitcher != nullptr && MainMenu != nullptr) {
        MenuSwitcher->SetActiveWidget(MainMenu);
    }
}

void UMainMenu::OnJoinClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Join Clicked"));
}
