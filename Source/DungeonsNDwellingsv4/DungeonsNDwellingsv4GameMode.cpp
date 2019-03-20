// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "DungeonsNDwellingsv4GameMode.h"
#include "DungeonsNDwellingsv4Pawn.h"

ADungeonsNDwellingsv4GameMode::ADungeonsNDwellingsv4GameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = ADungeonsNDwellingsv4Pawn::StaticClass();
}

