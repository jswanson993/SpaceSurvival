// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "InteractableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACESHIPSURVIVAL_API UInteractableComponent : public USphereComponent
{
	GENERATED_BODY()

private:
	UUserWidget* PromptWidget;
	class ASpaceShipSurvivalCharacter* Character;
protected:
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<UUserWidget> PromptWidgetClass;


public:	
	// Sets default values for this actor's properties
	UInteractableComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
