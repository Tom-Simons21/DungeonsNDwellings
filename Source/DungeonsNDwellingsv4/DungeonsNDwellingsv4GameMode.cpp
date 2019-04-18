// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "DungeonsNDwellingsv4GameMode.h"
#include "DungeonsNDwellingsv4.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "Blueprint/UserWidget.h"

void ADungeonsNDwellingsv4GameMode::BeginPlay()
{
	Super::BeginPlay();

	ADungeonsNDwellingsv4Pawn* playerCharacter = Cast<ADungeonsNDwellingsv4Pawn>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerHUDClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);

		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

ADungeonsNDwellingsv4GameMode::ADungeonsNDwellingsv4GameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = ADungeonsNDwellingsv4Pawn::StaticClass();
}

