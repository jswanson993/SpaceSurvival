// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerLine.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHIPSURVIVAL_API UServerLine : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* ServerLineButton;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerNameText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerTypeText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayersText;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsSelected = false;
	bool bRequiresPassword = false;
	FString Password = "";


	void Setup(class UMainMenu* Owner, int32 LineNum);
private:
	int32 Index;
	class UMainMenu* Parent;

	UFUNCTION()
	void OnLineSelected();
};
