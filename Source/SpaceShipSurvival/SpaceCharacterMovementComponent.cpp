// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceCharacterMovementComponent.h"
#include "GameFramework/Character.h"

void USpaceCharacterMovementComponent::UpdateBasedMovement(float DeltaSeconds)
{
	if (CharacterOwner->GetRootComponent() == nullptr || CharacterOwner->GetAttachParentActor() == nullptr) {
		Super::UpdateBasedMovement(DeltaSeconds);
	}
}
