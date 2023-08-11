// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuSystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include <Interfaces/OnlineFriendsInterface.h>
#include "Interfaces/OnlineIdentityInterface.h"
#include "SpaceShipSurvivalGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API USpaceShipSurvivalGameInstance : public UGameInstance, public IMenuSystem
{
	GENERATED_BODY()

public:
	USpaceShipSurvivalGameInstance(const FObjectInitializer& ObjectInitializer);

	void Init() override;

	void HostGame(FString ServerName, FString Password, FString ServerType, int32 PlayerLimit) override;

	void JoinGame(uint32 Index) override;

	void FindSessions(bool FriendsOnly) override;

	UFUNCTION(BlueprintCallable)
	void LoadMenu();

	UFUNCTION()
	void LoadInGameMenu();

	void LeaveGame();

private:
	TSubclassOf<class UUserWidget> MainMenuClass;
	class UMainMenu* MainMenu;
	TSubclassOf<class UUserWidget> InGameMenuClass;
	class UInGameMenu* InGameMenu;
	FString DesiredServerName;
	FString DesiredPassword;
	FString DesiredServerType;
	int32 DesiredPlayerLimit;
	IOnlineSessionPtr _OnlineSession;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	IOnlineFriendsPtr _OnlineFriends;
	IOnlineIdentityPtr _OnlineIdentity;
	FString FriendsListName = "Default";

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnFindFriendSessionsComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& FriendSearchResult);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type EOnJoinSessionCompleteResult);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);
	void CreateSession();
	TArray<FUniqueNetIdRef> GetFriendIds(const TArray<TSharedRef<FOnlineFriend>> &FriendsList);
};
