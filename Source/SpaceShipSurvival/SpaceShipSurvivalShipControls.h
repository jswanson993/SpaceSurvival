// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/Interactable.h"
#include "GameFramework/Actor.h"
#include "SpaceShipSurvivalShipControls.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API ASpaceShipSurvivalShipControls : public AInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category=Collision)
	class UBoxComponent* Collider;
	UPROPERTY(EditAnywhere, Category=Mesh)
	UStaticMeshComponent* Seat;

	UPROPERTY(ReplicatedUsing = OnRep_SetShip, EditInstanceOnly, Category=Pawn, meta = (AllowPrivateAccess = "true"))
	class ASpaceShipSurvivalShip* Ship;

public:
	ASpaceShipSurvivalShipControls();

protected:
	virtual void BeginPlay() override;
	virtual void Interact_Implementation(class APlayerController* PlayerController) override;

public:
	UFUNCTION(BlueprintCallable)
	void SetShip(class ASpaceShipSurvivalShip* NewShip) { OnRep_SetShip(NewShip); }
private:
	UFUNCTION()
	void OnRep_SetShip(class ASpaceShipSurvivalShip* NewShip) { Ship = NewShip; }
	bool CheckIfBeingUsed();

	UFUNCTION()
	void OnExitShip();
};
