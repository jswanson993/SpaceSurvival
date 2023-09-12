// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS(Abstract, Blueprintable)
class SPACESHIPSURVIVAL_API AInteractable : public AActor
{
	GENERATED_BODY()

private:
	UUserWidget* PromptWidget;
	class ASpaceShipSurvivalCharacter* Character;
	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	class USphereComponent* InteractVolume;

protected:
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> PromptWidgetClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	AInteractable();

	UFUNCTION(BlueprintNativeEvent)
	void Interact(class APlayerController* PlayerController);

	virtual void Interact_Implementation(class APlayerController* PlayerController) { return; }

		UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};