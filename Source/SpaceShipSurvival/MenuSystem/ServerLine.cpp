// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerLine.h"
#include "MainMenu.h"
#include "Components/Button.h"

void UServerLine::Setup(UMainMenu* Owner, int32 LineNum)
{
	AddToViewport();
	Parent = Owner;
	Index = LineNum;
	ServerLineButton->OnClicked.AddDynamic(this ,&UServerLine::OnLineSelected);

}

void UServerLine::OnLineSelected()
{
	Parent->UpdateSelection(Index);
}
