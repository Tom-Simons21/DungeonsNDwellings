// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "DungeonsNDwellingsv4GameMode.h"
#include "DungeonsNDwellingsv4.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "Blueprint/UserWidget.h"

void ADungeonsNDwellingsv4GameMode::BeginPlay()
{
	Super::BeginPlay();
	mapName = GetWorld()->GetMapName();

	ADungeonsNDwellingsv4Pawn* playerCharacter = Cast<ADungeonsNDwellingsv4Pawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (PlayerHUDClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
	if (mapName.Contains("Dwelling"))
	{
		DisplayLoadingScreen();
	}
}

ADungeonsNDwellingsv4GameMode::ADungeonsNDwellingsv4GameMode()
{
	DefaultPawnClass = ADungeonsNDwellingsv4Pawn::StaticClass();
}

void ADungeonsNDwellingsv4GameMode::DisplayLoadingScreen()
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	if (LoadingScreenClass != nullptr)
	{
		LoadingWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenClass);
		if (LoadingWidget != nullptr)
		{
			LoadingWidget->AddToViewport();
		}
	}
}

void ADungeonsNDwellingsv4GameMode::RemoveLoadingScreen()
{
	if (LoadingWidget != nullptr)
	{
		LoadingWidget->RemoveFromViewport();
	}
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

