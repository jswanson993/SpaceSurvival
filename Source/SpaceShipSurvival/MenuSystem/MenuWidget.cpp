// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

void UMenuWidget::SetMenuSystem(IMenuSystem* MenuSystem)
{
    _MenuSystem = MenuSystem;
}

void UMenuWidget::Setup()
{
    this->AddToViewport();

    UWorld* world = GetWorld();
    if (!ensure(world != nullptr)) return;

    APlayerController* playerController = world->GetFirstPlayerController();
    if (!ensure(playerController != nullptr)) return;
    //Set input to ui only and focus on menu
    FInputModeUIOnly inputMode;
    inputMode.SetWidgetToFocus(this->TakeWidget());
    inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    playerController->SetInputMode(inputMode);

    playerController->SetShowMouseCursor(true);
}

void UMenuWidget::TearDown()
{
    UWorld* world = GetWorld();
    if (!ensure(world != nullptr)) return;

    APlayerController* playerController = world->GetFirstPlayerController();
    if (!ensure(playerController != nullptr)) return;

    playerController->SetShowMouseCursor(false);
    //Set input back to game
    FInputModeGameOnly inputMode;
    playerController->SetInputMode(inputMode);
    //Remove widget from screen
    RemoveFromParent();
}

/*Quit the game*/
void UMenuWidget::OnQuitClicked()
{
    UWorld* world = GetWorld();
    if (!ensure(world != nullptr)) return;

    APlayerController* playerController = world->GetFirstPlayerController();
    if (!ensure(playerController != nullptr)) return;
    playerController->ConsoleCommand("Exit");

}