// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "DungeonsNDwellingsv4GameMode.h"
#include "DungeonsNDwellingsv4.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "Blueprint/UserWidget.h"

void ADungeonsNDwellingsv4GameMode::BeginPlay()
{
	Super::BeginPlay();
	mapName = GetWorld()->GetMapName();		//get the level name on start

	ADungeonsNDwellingsv4Pawn* playerCharacter = Cast<ADungeonsNDwellingsv4Pawn>(UGameplayStatics::GetPlayerPawn(this, 0)); //setup player pawn
	if (PlayerHUDClass != nullptr)	//check player hud has a value
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);	//set up the current widget
		if (CurrentWidget != nullptr)					//check is not null
		{
			CurrentWidget->AddToViewport();			//add to screen
		}
	}
	if (mapName.Contains("Dwelling"))		//on all levels, not menus, add loading screen at start
	{
		DisplayLoadingScreen();			//add loading screen
	}
}

ADungeonsNDwellingsv4GameMode::ADungeonsNDwellingsv4GameMode()
{
	DefaultPawnClass = ADungeonsNDwellingsv4Pawn::StaticClass();
}

void ADungeonsNDwellingsv4GameMode::DisplayLoadingScreen() //display the loading screen
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);  //pause game while loading screen is displayed

	if (LoadingScreenClass != nullptr)	//checks loading screen is valid
	{
		LoadingWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenClass); //setup widget
		if (LoadingWidget != nullptr)	//check is not null
		{
			LoadingWidget->AddToViewport();	//add the loading widget to screen
		}
	}
}

void ADungeonsNDwellingsv4GameMode::RemoveLoadingScreen() //function to remove loading screen
{
	if (LoadingWidget != nullptr)	//check is not null
	{
		LoadingWidget->RemoveFromViewport(); //remove from screen
	}
	UGameplayStatics::SetGamePaused(GetWorld(), false); //unpause
}

