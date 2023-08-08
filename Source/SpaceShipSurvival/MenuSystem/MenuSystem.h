// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MenuSystem.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMenuSystem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPACESHIPSURVIVAL_API IMenuSystem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void HostGame(FString ServerName, FString Password, FString ServerType, int32 PlayerLimit) = 0;
	virtual void JoinGame() = 0;
	virtual void FindSessions(bool FriendsOnly) = 0;
};
