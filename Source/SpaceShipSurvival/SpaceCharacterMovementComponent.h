// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SpaceCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API USpaceCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	virtual void UpdateBasedMovement(float DeltaSeconds) override;
	
};
