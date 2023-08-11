// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/Scrollbox.h"
#include "Components/TextBlock.h"
#include "UObject/ConstructorHelpers.h"

#include "ServerLine.h"
#include "CustomButton.h"


UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer){
    //Find the Main Menu Widget in the content browser
    static ConstructorHelpers::FClassFinder<UUserWidget> ServerLineClassFinder(TEXT("/Game/MenuSystem/WBP_ServerLine"));
    if (ServerLineClassFinder.Class != nullptr) {
        ServerLineClass = ServerLineClassFinder.Class;
    }
}

bool UMainMenu::Initialize()
{
    bool success = Super::Initialize();
    if(!success) return false;

    //Bind On Click Events
    if(!ensure(HostGameButton != nullptr)) return false;
    HostGameButton->CustomButton->OnClicked.AddDynamic(this, &UMainMenu::OnHostGameClicked);
    if (!ensure(JoinGameButton != nullptr)) return false;
    JoinGameButton->CustomButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinGameClicked);
    if (!ensure(OptionsButton != nullptr)) return false;
    OptionsButton->CustomButton->OnClicked.AddDynamic(this, &UMainMenu::OnOptionsClicked);
    if (!ensure(QuitButton != nullptr)) return false;
    QuitButton->CustomButton->OnClicked.AddDynamic(this, &UMenuWidget::OnQuitClicked);
    if (!ensure(StartGameButton != nullptr)) return false;
    StartGameButton->CustomButton->OnClicked.AddDynamic(this, &UMainMenu::OnStartGameClicked);
    if (!ensure(BackButton != nullptr)) return false;
    BackButton->CustomButton->OnClicked.AddDynamic(this, &UMainMenu::OnBackClicked);
    if (!ensure(JoinButton != nullptr)) return false;
    JoinButton->CustomButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinClicked);
    if (!ensure(BackButton2 != nullptr)) return false;
    BackButton2->CustomButton->OnClicked.AddDynamic(this, &UMainMenu::OnBackClicked);

    return true;
}

void UMainMenu::SetServerList(TArray<FServerDetails> Servers)
{
    ServerList->ClearChildren();
    UWorld* world = GetWorld();
    FoundServers = Servers;
    int32 index = 0;
    for (FServerDetails &server : Servers) {
        //Create server line
        UServerLine* serverLine = CreateWidget<UServerLine>(world, ServerLineClass);
        if(serverLine != nullptr){
            UE_LOG(LogTemp, Warning, TEXT("Creating server line %s: "), *server.ServerName);
            serverLine->bIsSelected = false;
            serverLine->ServerNameText->SetText(FText::FromString(server.ServerName));
            serverLine->ServerTypeText->SetText(FText::FromString(server.ServerType));
            serverLine->PlayersText->SetText(FText::FromString(server.Players));
            if (server.ServerType == "Private") {
                serverLine->bRequiresPassword = true;
                serverLine->Password = server.ServerPassword;
            }
            serverLine->Setup(this, index);
            //Add to Server List
            ServerList->AddChild(serverLine);
        }
        index++;

    }
}

void UMainMenu::UpdateSelection(int32 Index)
{
    for (int32 i = 0; i < ServerList->GetChildrenCount(); i++) {
        auto line = Cast<UServerLine>(ServerList->GetChildAt(i));
        if (line != nullptr) {
            if (i == Index) {
                line->bIsSelected = true;
            }
            else {
                line->bIsSelected = false;
            }
        }
    }
}



/*Open Server Creation Menu*/
void UMainMenu::OnHostGameClicked()
{
    if (MenuSwitcher != nullptr && HostMenu != nullptr){
        MenuSwitcher->SetActiveWidget(HostMenu);
    }
}

/*Open Server Selection menu*/
void UMainMenu::OnJoinGameClicked()
{
    if (MenuSwitcher != nullptr && JoinMenu != nullptr) {
        MenuSwitcher->SetActiveWidget(JoinMenu);
        _MenuSystem->FindSessions(false);
    }
}
/*Open Options menu*/
void UMainMenu::OnOptionsClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Clicked Options"));
}

/*Create new server*/
void UMainMenu::OnStartGameClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Clicked Start Game"));
    if (_MenuSystem != nullptr && ServerNameTextBox != nullptr && PasswordTextBox != nullptr) {
        //Get server setup details
        FString serverName = ServerNameTextBox->GetText().ToString();
        FString serverPassword = PasswordTextBox->GetText().ToString();
        FString serverType = ServerTypeDropdown->GetSelectedOption();
        int32 playerLimit = PlayerLimitSlider->GetValue();
        _MenuSystem->HostGame(serverName, serverPassword, serverType, playerLimit);
    }
}
/*Return to main menu*/
void UMainMenu::OnBackClicked()
{
    if (MenuSwitcher != nullptr && MainMenu != nullptr) {
        MenuSwitcher->SetActiveWidget(MainMenu);
    }
}
/*Join Selected Server*/
void UMainMenu::OnJoinClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Join Clicked"));
}
