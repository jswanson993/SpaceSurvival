// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipSurvivalGameInstance.h"
#include "MenuSystem/MainMenu.h"
#include "OnlineSessionSettings.h"
#include "UObject/ConstructorHelpers.h"


USpaceShipSurvivalGameInstance::USpaceShipSurvivalGameInstance(const FObjectInitializer& ObjectInitializer)
{
	//Find the Main Menu Widget in the content browser
	static ConstructorHelpers::FClassFinder<UMainMenu> MainMenuClassFinder(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (MainMenuClassFinder.Class != nullptr) {
		MainMenuClass = MainMenuClassFinder.Class;
	}
}

void USpaceShipSurvivalGameInstance::Init()
{
	Super::Init();
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if (onlineSubsystem != nullptr){
		_OnlineSession = onlineSubsystem->GetSessionInterface();
		//Bind Delegates
		if (_OnlineSession.IsValid()) {
			_OnlineSession->OnCreateSessionCompleteDelegates.AddUObject(this, &USpaceShipSurvivalGameInstance::OnCreateSessionComplete);
			_OnlineSession->OnDestroySessionCompleteDelegates.AddUObject(this, &USpaceShipSurvivalGameInstance::OnDestroySessionComplete);
		}
	}



}

void USpaceShipSurvivalGameInstance::HostGame(FString ServerName, FString Password, FString ServerType, int32 PlayerLimit)
{
	if(!ensure(_OnlineSession != nullptr)) return;

	//Set server details in case server already exists
	DesiredServerName = ServerName;
	DesiredPassword = Password;
	DesiredServerType = ServerType;
	DesiredPlayerLimit = PlayerLimit;

	//Check to see if the session was destroyed before starting a new one
	FNamedOnlineSession* foundSession =_OnlineSession->GetNamedSession(NAME_GameSession);
	if (foundSession != nullptr) {
		_OnlineSession->DestroySession(NAME_GameSession);
		return;
	}
	
	CreateSession();
}

void USpaceShipSurvivalGameInstance::JoinGame()
{
	if (MainMenu != nullptr) {
		MainMenu->TearDown();
	}
}

void USpaceShipSurvivalGameInstance::LoadMenu()
{
	if(!ensure(MainMenuClass != nullptr)) return;
	MainMenu = CreateWidget<UMainMenu>(this, MainMenuClass);
	if(!ensure(MainMenu != nullptr)) return;
	MainMenu->SetMenuSystem(this);
	
	MainMenu->Setup();
}

void USpaceShipSurvivalGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(MainMenu != nullptr && bWasSuccessful){
		MainMenu->TearDown();
		UWorld* world = GetWorld();
		world->ServerTravel("/Game/FirstPerson/Maps/FirstPersonMap");
	}
	else if (!bWasSuccessful) {
		UE_LOG(LogTemp, Warning, TEXT("Could not create Sesion"));
	}
}

void USpaceShipSurvivalGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	//Recreate the session after its been destoryed
	if (bWasSuccessful) {
		CreateSession();
	}
}

void USpaceShipSurvivalGameInstance::CreateSession() {

		FOnlineSessionSettings sessionSettings;
		sessionSettings.bAllowInvites = true;
		sessionSettings.bAllowJoinInProgress = true;
		sessionSettings.bAllowJoinViaPresence = true;
		sessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL"; //Use lan connection if testing
		sessionSettings.bUseLobbiesIfAvailable = true;
		sessionSettings.bUsesPresence = true;

		if (DesiredServerType.Equals("Public")) {
			sessionSettings.NumPublicConnections = DesiredPlayerLimit;
			sessionSettings.NumPrivateConnections = 0;
		}
		else {
			sessionSettings.NumPrivateConnections = DesiredPlayerLimit;
			sessionSettings.NumPublicConnections = 0;
			if (DesiredServerType.Equals("Friends Only")) {
				sessionSettings.bAllowJoinViaPresenceFriendsOnly = true;
			}
		}

		sessionSettings.Set("ServerName", DesiredServerName);
		sessionSettings.Set("ServerPassword", DesiredPassword);
		_OnlineSession->CreateSession(0, NAME_GameSession, sessionSettings);
}
