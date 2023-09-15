// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpaceSurvivalCharacterController.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API ASpaceSurvivalCharacterController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASpaceSurvivalCharacterController();

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UFUNCTION()
	void LoadMenu();

	UFUNCTION(Server, Reliable)
	void Server_Possess(APawn* NewPawn);

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MenuAction;
;


};
