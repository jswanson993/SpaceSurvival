// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipSurvivalGameInstance.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/InGameMenu.h"
#include "OnlineSessionSettings.h"
#include "UObject/ConstructorHelpers.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>


const FName SERVER_NAME_KEY = "ServerName";
const FName PASSWORD_KEY = "Passowrd";

USpaceShipSurvivalGameInstance::USpaceShipSurvivalGameInstance(const FObjectInitializer& ObjectInitializer)
{
	//Find the Main Menu Widget in the content browser
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuClassFinder(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (MainMenuClassFinder.Class != nullptr) {
		MainMenuClass = MainMenuClassFinder.Class;
	}

	//Find the Main Menu Widget in the content browser
	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuClassFinder(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
	if (InGameMenuClassFinder.Class != nullptr) {
		InGameMenuClass = InGameMenuClassFinder.Class;
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
			_OnlineSession->OnFindSessionsCompleteDelegates.AddUObject(this, &USpaceShipSurvivalGameInstance::OnFindSessionsComplete);
			_OnlineSession->OnFindFriendSessionCompleteDelegates->AddUObject(this, &USpaceShipSurvivalGameInstance::OnFindFriendSessionsComplete);
			_OnlineSession->OnJoinSessionCompleteDelegates.AddUObject(this, &USpaceShipSurvivalGameInstance::OnJoinSessionComplete);
		}
	}

	if (GEngine) {
		GEngine->OnNetworkFailure().AddUObject(this, &USpaceShipSurvivalGameInstance::OnNetworkFailure);
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

void USpaceShipSurvivalGameInstance::JoinGame(uint32 Index)
{
	if(!SessionSearch.IsValid()) return;
	if(!_OnlineSession.IsValid()) return;

	if (MainMenu != nullptr) {
		MainMenu->TearDown();
	}

	_OnlineSession->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[Index]);

}

void USpaceShipSurvivalGameInstance::FindSessions(bool FriendsOnly)
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if(SessionSearch.IsValid()){
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionSearch->MaxSearchResults = 100;
		if (_OnlineSession.IsValid()) {
			if(FriendsOnly){
				//_OnlineSession
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Starting Session Search"));
				_OnlineSession->FindSessions(0, SessionSearch.ToSharedRef());
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Could not start session search"));
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

void USpaceShipSurvivalGameInstance::LoadInGameMenu()
{
	if(!ensure(InGameMenuClass != nullptr)) return;
	InGameMenu = CreateWidget<UInGameMenu>(this, InGameMenuClass);
	if(!ensure(InGameMenu != nullptr)) return
	InGameMenu->SetMenuSystem(this);

	InGameMenu->Setup();
	InGameMenu->SetMenuSystem(this);
}

void USpaceShipSurvivalGameInstance::LeaveGame()
{
	UWorld* world = GetWorld();
	if (!ensure(world != nullptr)) return;

	APlayerController* playerController = world->GetFirstPlayerController();
	if (!ensure(playerController != nullptr)) return;

	playerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);

}

void USpaceShipSurvivalGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(MainMenu != nullptr && bWasSuccessful && _OnlineSession != nullptr){
		MainMenu->TearDown();
		UWorld* world = GetWorld();
		_OnlineSession->StartSession(SessionName);
		world->ServerTravel("/Game/FirstPerson/Maps/FirstPersonMap?listen");
	}
	else if (!bWasSuccessful) {
		UE_LOG(LogTemp, Warning, TEXT("Could not create Sesion"));
	}
}

void USpaceShipSurvivalGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	//Recreate the session after its been destroyed
	if (bWasSuccessful) {
		CreateSession();
	}
}

void USpaceShipSurvivalGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session Search Complete"));
	TArray<FServerDetails> foundServers;
	if (MainMenu != nullptr && bWasSuccessful && SessionSearch.IsValid()) {
		for (auto sessionResult : SessionSearch->SearchResults) {
			if (sessionResult.IsValid()) {
				FServerDetails serverDetails;
				FString serverName;
				FString serverPassword;
				if (sessionResult.Session.SessionSettings.Get("ServerName", serverName)) {
					serverDetails.ServerName = serverName;
				}
				else {
					serverDetails.ServerName = "Unknown";
				}

				if (sessionResult.Session.SessionSettings.Get("ServerPassword", serverPassword)) {
					serverDetails.ServerPassword = serverPassword;
				}
				else {
					serverDetails.ServerPassword = "Unknown";
				}
				if (sessionResult.Session.SessionSettings.NumPublicConnections == 0) {
					serverDetails.ServerType = "Private";
					serverDetails.Players = FString::Printf(TEXT("%d/%d"),
					sessionResult.Session.NumOpenPrivateConnections,
					sessionResult.Session.SessionSettings.NumPrivateConnections);
				}
				else {
					serverDetails.ServerType = "Public";
					serverDetails.Players = FString::Printf(TEXT("%d/%d"),
					sessionResult.Session.NumOpenPublicConnections,
					sessionResult.Session.SessionSettings.NumPublicConnections);
				}
				foundServers.Add(serverDetails);
			}
		}
		MainMenu->SetServerList(foundServers);

	}
}

void USpaceShipSurvivalGameInstance::OnFindFriendSessionsComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& FriendSearchResult)
{
	if (MainMenu != nullptr) {
		MainMenu->TearDown();
	}
}

void USpaceShipSurvivalGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type EOnJoinSessionCompleteResult)
{
	if (EOnJoinSessionCompleteResult == EOnJoinSessionCompleteResult::Success || EOnJoinSessionCompleteResult == EOnJoinSessionCompleteResult::AlreadyInSession) {
		if(!_OnlineSession.IsValid()) return;
		
		FString ConnectInfo;
		if(!_OnlineSession->GetResolvedConnectString(SessionName, ConnectInfo)) return;
		
		APlayerController* playerController = GetFirstLocalPlayerController();
		if(!ensure(playerController != nullptr)) return;
		playerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute, true);
	}

}

void USpaceShipSurvivalGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	LeaveGame();
}

void USpaceShipSurvivalGameInstance::CreateSession() {

		FOnlineSessionSettings sessionSettings;
		sessionSettings.bAllowInvites = true;
		sessionSettings.bAllowJoinInProgress = true;
		sessionSettings.bAllowJoinViaPresence = true;
		sessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL"; //Use lan connection if testing
		sessionSettings.bUseLobbiesIfAvailable = true;
		sessionSettings.bShouldAdvertise = true;
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
		UE_LOG(LogTemp, Warning, TEXT("%s"), *DesiredPassword)
		sessionSettings.Set(SERVER_NAME_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		sessionSettings.Set(PASSWORD_KEY, DesiredPassword, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		_OnlineSession->CreateSession(0, NAME_GameSession, sessionSettings);
}
