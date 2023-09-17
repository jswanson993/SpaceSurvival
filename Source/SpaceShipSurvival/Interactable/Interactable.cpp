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
	Character = Cast<ASpaceShipSurvivalCharacter>(OtherActor);
	if (Character != nullptr && Character->IsLocallyControlled()) {
		UWorld* world = GetWorld();
		if (world != nullptr && PromptWidgetClass != nullptr) {
			if (PromptWidget == nullptr) {
				PromptWidget = CreateWidget<UUserWidget>(world, PromptWidgetClass);
			}
			if (!PromptWidget->IsInViewport()) {
				PromptWidget->AddToViewport();
			}
		}
			
		Character->OnInteract.AddDynamic(this, &AInteractable::Interact);
		
	}
}

void AInteractable::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	auto OtherCharacter = Cast<ASpaceShipSurvivalCharacter>(OtherActor);

	if(OtherCharacter != nullptr && OtherCharacter->IsLocallyControlled() && OtherCharacter == Character){
		if(PromptWidget != nullptr && PromptWidget->IsInViewport()){
			PromptWidget->RemoveFromParent();
		}

		if (Character != nullptr) {
			Character->OnInteract.RemoveAll(this);
		}
	}

}


