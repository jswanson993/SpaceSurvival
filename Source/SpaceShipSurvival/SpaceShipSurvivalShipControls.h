// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/InteractableBase.h"
#include "SpaceShipSurvivalShipControls.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API ASpaceShipSurvivalShipControls : public AInteractableBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Collision)
	class UBoxComponent* Collider;
	UPROPERTY(EditAnywhere, Category=Mesh)
	UStaticMeshComponent* Seat;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category=Pawn, meta = (AllowPrivateAccess = "true"))
	APawn* Ship;

	UPROPERTY(Replicated)
	bool bIsBeingUsed = false;
public:
	ASpaceShipSurvivalShipControls();

protected:
	virtual void BeginPlay() override;
	virtual void Interact(class APlayerController* PlayerController) override;

public:

private:
	
};
