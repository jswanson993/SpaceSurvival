// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "InteractableBase.generated.h"


UCLASS()
class SPACESHIPSURVIVAL_API AInteractableBase : public AActor, public IInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category=Trigger)
	class USphereComponent* SphereCollider;

	UUserWidget* PromptWidget;
	class ASpaceShipSurvivalCharacter* Character;
protected:
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> PromptWidgetClass;

	TSubclassOf<AInteractableBase> ChildClass;


public:	
	// Sets default values for this actor's properties
	AInteractableBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void Interact(class APlayerController* PlayerController) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
};
