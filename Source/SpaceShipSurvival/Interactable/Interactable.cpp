// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "SpaceShipSurvival/SpaceShipSurvivalCharacter.h"

// Add default functionality here for any IInteractable functions that are not pure virtual.

AInteractable::AInteractable()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	InteractVolume = CreateDefaultSubobject<USphereComponent>(TEXT("Interact Volume"));
	InteractVolume->SetupAttachment(RootComponent);
}

void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	InteractVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractable::BeginOverlap);
	InteractVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractable::EndOverlap);
}

void AInteractable::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpaceShipSurvivalCharacter* OtherCharacter = Cast<ASpaceShipSurvivalCharacter>(OtherActor);

	//Verify that the overlapping character is a local player and they are not part currently included in the set of characters inside the overlap
	if (OtherCharacter != nullptr && OtherCharacter->IsLocallyControlled() && !Characters.Contains(OtherCharacter)) {
		Characters.Add(OtherCharacter);
		UWorld* world = GetWorld();
		if (world != nullptr && PromptWidgetClass != nullptr) {
			if (PromptWidget == nullptr) {
				PromptWidget = CreateWidget<UUserWidget>(world, PromptWidgetClass);
			}
			if (!PromptWidget->IsInViewport()) {
				PromptWidget->AddToViewport();
			}
		}
		if (!OtherCharacter->OnInteract.GetAllObjects().Contains(this)) {
			OtherCharacter->OnInteract.AddDynamic(this, &AInteractable::Interact);
		}
		
	}
}

void AInteractable::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	auto OtherCharacter = Cast<ASpaceShipSurvivalCharacter>(OtherActor);

	//Verify that the no longer overlapping character is a local player and that they were included in the set of characters inside the overlap
	if(OtherCharacter != nullptr && OtherCharacter->IsLocallyControlled() && Characters.Contains(OtherCharacter)){
		if(PromptWidget != nullptr && PromptWidget->IsInViewport()){
			PromptWidget->RemoveFromParent();
		}

		if (OtherCharacter != nullptr) {
			OtherCharacter->OnInteract.RemoveAll(this);
		}

		Characters.Remove(OtherCharacter);
	}

}


