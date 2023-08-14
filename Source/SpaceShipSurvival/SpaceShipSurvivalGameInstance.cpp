// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShipSurvivalGameInstance.h"
#include "MenuSystem/InGameMenu.h"

#include "UObject/ConstructorHelpers.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>


const FName SERVER_NAME_KEY = "ServerName";
const FName PASSWORD_KEY = "Passowrd";
const FName SERVER_TYPE_KEY = "ServerType";

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
		UE_LOG(LogTemp, Warning, TEXT("Found Subsystem: %s"), *onlineSubsystem->GetSubsystemName().ToString());
		_OnlineSession = onlineSubsystem->GetSessionInterface();
		_OnlineFriends = onlineSubsystem->GetFriendsInterface();
		_OnlineIdentity = onlineSubsystem->GetIdentityInterface();
		_OnlineUser = onlineSubsystem->GetUserInterface();
		//Bind Delegates
		if (_OnlineSession.IsValid()) {
			_OnlineSession->OnCreateSessionCompleteDelegates.AddUObject(this, &USpaceShipSurvivalGameInstance::OnCreateSessionComplete);
			_OnlineSession->OnDestroySessionCompleteDelegates.AddUObject(this, &USpaceShipSurvivalGameInstance::OnDestroySessionComplete);
			_OnlineSession->OnStartSessionCompleteDelegates.AddUObject(this, &USpaceShipSurvivalGameInstance::OnStartSessionComplete);
			_OnlineSession->OnFindSessionsCompleteDelegates.AddUObject(this, &USpaceShipSurvivalGameInstance::OnFindSessionsComplete);
			_OnlineSession->OnFindFriendSessionCompleteDelegates->AddUObject(this, &USpaceShipSurvivalGameInstance::OnFindFriendSessionComplete);
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
	UE_LOG(LogTemp, Warning, TEXT("Trying To Join Index %d"), Index);
	if(!SessionSearch.IsValid() && FriendSearchResults.IsEmpty()) return;
	if(!_OnlineSession.IsValid()) return;

	if (MainMenu != nullptr) {
		MainMenu->TearDown();
	}
	FOnlineSessionSearchResult session;
	if (SessionSearch.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("Using Session Search"));
		session = SessionSearch->SearchResults[Index];
	}
	else if (!FriendSearchResults.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("Using Friend Session Search"));
		session = FriendSearchResults[Index];
	}
	_OnlineSession->JoinSession(0, NAME_GameSession, session);

}

void USpaceShipSurvivalGameInstance::FindSessions(bool FriendsOnly)
{
	//FriendsOnly = true; //TODO: Remove after testing

	if (_OnlineSession.IsValid()) {
		if(FriendsOnly){				
			_OnlineFriends->ReadFriendsList(0, FriendsListName, FOnReadFriendsListComplete::CreateUObject(this, &USpaceShipSurvivalGameInstance::OnReadFriendsListComplete));
		}
		else {
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			//SessionSearch
			if (SessionSearch.IsValid()) {
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
				SessionSearch->MaxSearchResults = 100;
				UE_LOG(LogTemp, Warning, TEXT("Starting Session Search"));
				_OnlineSession->FindSessions(0, SessionSearch.ToSharedRef());
			}
		}
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
		_OnlineSession->StartSession(SessionName);
	}
	else if (!bWasSuccessful) {
		UE_LOG(LogTemp, Warning, TEXT("Could not create Session"));
	}
}

void USpaceShipSurvivalGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	//Recreate the session after its been destroyed
	if (bWasSuccessful) {
		CreateSession();
	}
}

void USpaceShipSurvivalGameInstance::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful) {
		UWorld* world = GetWorld();
		world->ServerTravel("/Game/FirstPerson/Maps/FirstPersonMap?listen");
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
				FString serverType;
				if (sessionResult.Session.SessionSettings.Get(SERVER_NAME_KEY, serverName)) {
					serverDetails.ServerName = serverName;
					UE_LOG(LogTemp, Warning, TEXT("Found Server: %s"), *serverName);
				}
				else {
					serverDetails.ServerName = "Unknown";
				}

				if (sessionResult.Session.SessionSettings.Get(PASSWORD_KEY, serverPassword)) {
					serverDetails.ServerPassword = serverPassword;
					UE_LOG(LogTemp, Warning, TEXT("Found Password: %s"), *serverPassword);
				}
				else {
					serverDetails.ServerPassword = "";
				}
				if (sessionResult.Session.SessionSettings.Get(SERVER_NAME_KEY, serverType)) {
					if(!serverType.Equals("Public")){
						serverDetails.ServerType = "Private";
						serverDetails.Players = FString::Printf(TEXT("%d/%d"), sessionResult.Session.SessionSettings.NumPrivateConnections - sessionResult.Session.NumOpenPrivateConnections,
							sessionResult.Session.SessionSettings.NumPublicConnections);
					}else{
						serverDetails.ServerType = "Public";
						serverDetails.Players = FString::Printf(TEXT("%d/%d"), sessionResult.Session.SessionSettings.NumPrivateConnections - sessionResult.Session.NumOpenPrivateConnections,
							sessionResult.Session.SessionSettings.NumPrivateConnections);
					}
					
				}



				foundServers.Add(serverDetails);
			}
		}
		MainMenu->SetServerList(foundServers);

	}
}

void USpaceShipSurvivalGameInstance::OnFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& FriendSearchResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Finding Friend Game"));
	if (MainMenu != nullptr && bWasSuccessful) {
		for (auto friendSearchResult : FriendSearchResult) {
			FriendSearchResults.Add(friendSearchResult);
			FServerDetails serverDetails;
			serverDetails.OwningUser = friendSearchResult.Session.OwningUserName;
			FString serverName;
			if(friendSearchResult.Session.SessionSettings.Get(SERVER_NAME_KEY, serverName)){
				serverDetails.ServerName = serverName;
			}
			else {
				serverDetails.ServerName = "Unknown";
			}
			serverDetails.ServerPassword = "";
			serverDetails.Players = FString::Printf(TEXT("%d/%d"), (friendSearchResult.Session.SessionSettings.NumPrivateConnections - friendSearchResult.Session.NumOpenPrivateConnections), (friendSearchResult.Session.SessionSettings.NumPrivateConnections));

			FriendServers.Add(serverDetails);
		}
	}

	if (FriendsListSearchIndex < FriendsList.Num() - 1) {
		if(_OnlineSession.IsValid()){
			++FriendsListSearchIndex;
			_OnlineSession->FindFriendSession(LocalUserNum, FriendsList[FriendsListSearchIndex]->GetUserId().Get());
		}
	}
	else {
		SendFriendServersToMenu();
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

void USpaceShipSurvivalGameInstance::OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	if (bWasSuccessful && _OnlineFriends.IsValid()) {
		bool success = _OnlineFriends->GetFriendsList(LocalUserNum, ListName, FriendsList);
		if (success) {
			UE_LOG(LogTemp, Warning, TEXT("Found %d Friends"), FriendsList.Num());
			MainMenu->ClearServerList();
			FUniqueNetIdPtr netID = _OnlineIdentity->GetUniquePlayerId(LocalUserNum);
			UE_LOG(LogTemp, Warning, TEXT("Starting Friend Search"));
			_OnlineSession->FindFriendSession(*netID, FriendsList[FriendsListSearchIndex]->GetUserId().Get());
		}
	}
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

		if (!DesiredServerType.Equals("Public")) {
			sessionSettings.NumPrivateConnections = DesiredPlayerLimit;
			sessionSettings.NumPublicConnections = 0;
		}
		else {
			sessionSettings.NumPublicConnections = DesiredPlayerLimit;
			sessionSettings.NumPrivateConnections = 0;
		}

		UE_LOG(LogTemp, Warning, TEXT("Setting Desired Password to %s"), *DesiredPassword)
		sessionSettings.Set(SERVER_NAME_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		sessionSettings.Set(PASSWORD_KEY, DesiredPassword, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		sessionSettings.Set(SERVER_TYPE_KEY, DesiredServerType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		_OnlineSession->CreateSession(0, NAME_GameSession, sessionSettings);
}

void USpaceShipSurvivalGameInstance::SendFriendServersToMenu()
{
	if (MainMenu != nullptr) {
		MainMenu->SetServerList(FriendServers);
	}

	FriendsListSearchIndex = 0;
}
