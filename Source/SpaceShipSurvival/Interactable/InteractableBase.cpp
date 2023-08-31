// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableBase.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "SpaceShipSurvival/SpaceShipSurvivalCharacter.h"

// Sets default values
AInteractableBase::AInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	SphereCollider->SetupAttachment(RootComponent);

}

// Called every frame
void AInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AInteractableBase::BeginOverlap);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AInteractableBase::EndOverlap);
}

void AInteractableBase::Interact(APlayerController* PlayerController)
{
	return;
}

void AInteractableBase::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
		Character->OnInteract.AddDynamic(this, &AInteractableBase::Interact);
	}
}

void AInteractableBase::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PromptWidget->RemoveFromParent();

	if (Character != nullptr) {
		Character->OnInteract.RemoveAll(this);
	}
}
