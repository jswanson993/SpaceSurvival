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

	UPROPERTY(EditAnywhere, Category=Collision)
	class UBoxComponent* Collider;
	UPROPERTY(EditAnywhere, Category=Mesh)
	UStaticMeshComponent* Seat;

	//UPROPERTY(EditAnywhere, Category = Interaction)
	//class UInteractableComponent* Interactable;

	UPROPERTY(ReplicatedUsing = OnRep_SetShip, EditInstanceOnly, Category=Pawn, meta = (AllowPrivateAccess = "true"))
	APawn* Ship;

	UPROPERTY(Replicated)
	bool bIsBeingUsed = false;
public:
	ASpaceShipSurvivalShipControls();

protected:
	virtual void BeginPlay() override;
	virtual void Interact_Implementation(class APlayerController* PlayerController) override;
	UFUNCTION(Server, Reliable)
	void Server_PossessShip(APlayerController* PlayerController);

public:
	UFUNCTION(BlueprintCallable)
	void SetShip(APawn* NewShip) { OnRep_SetShip(NewShip); }
private:
	UFUNCTION()
	void OnRep_SetShip(APawn* NewShip) { Ship = NewShip; }
};
