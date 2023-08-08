// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuSystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
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

	void JoinGame() override;

	void FindSessions(bool FriendsOnly) override;

	UFUNCTION(BlueprintCallable)
	void LoadMenu();

private:
	TSubclassOf<class UMainMenu> MainMenuClass;
	class UMainMenu* MainMenu;
	FString DesiredServerName;
	FString DesiredPassword;
	FString DesiredServerType;
	int32 DesiredPlayerLimit;

	IOnlineSessionPtr _OnlineSession;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnFindFriendSessionsComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& FriendSearchResult);

	void CreateSession();
	
};
