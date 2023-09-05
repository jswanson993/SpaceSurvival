// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Interactable.h"
#include "SpaceShipSurvival/SpaceShipSurvivalCharacter.h"

// Sets default values
UInteractableComponent::UInteractableComponent()
{
	
}


void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
	this->OnComponentBeginOverlap.AddDynamic(this, &UInteractableComponent::BeginOverlap);
	this->OnComponentEndOverlap.AddDynamic(this, &UInteractableComponent::EndOverlap);
}

void UInteractableComponent::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Character = Cast<ASpaceShipSurvivalCharacter>(OtherActor);
	if (Character != nullptr && Character->IsLocallyControlled()) {
		UWorld* world = GetWorld();
		if (world != nullptr && PromptWidgetClass != nullptr) {
			if(PromptWidget == nullptr){
				PromptWidget = CreateWidget<UUserWidget>(world, PromptWidgetClass);
			}
			if(!PromptWidget->IsInViewport()){
				PromptWidget->AddToViewport();
			}
		}
		IInteractable* owner = Cast<IInteractable>(this->GetOwner());
		if(owner != nullptr){
			Character->OnInteract.AddDynamic(owner, &IInteractable::Interact_Implementation);
		}
		else {
			FString warning = FString::Printf(TEXT("%s: Owning Actor, %s, does not inherit from base class IInteractable"), *this->GetReadableName(), *this->GetOwner()->GetHumanReadableName());
			UE_LOG(LogTemp, Warning, TEXT("Warning: %s"), *warning);
			FMessageLog(FName("PIE")).Warning(FText::FromString(warning));
		}
	}
}

void UInteractableComponent::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PromptWidget->RemoveFromParent();

	if (Character != nullptr) {
		Character->OnInteract.RemoveAll(this);
	}
}
