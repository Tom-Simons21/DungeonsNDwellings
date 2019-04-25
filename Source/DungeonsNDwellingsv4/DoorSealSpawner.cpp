// Fill out your copyright notice in the Description page of Project Settings.

#include "DoorSealSpawner.h"
#include "DoorSeal.h"
#include "Engine.h"
#include "Engine/World.h"
#include "TileGeneratorParent.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "DungeonsNDwellingsv4GameMode.h"

// Sets default values
ADoorSealSpawner::ADoorSealSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SetTickableWhenPaused(true);

	doorSealArray.Empty();
	roomCount = 0;
	roomsOpened = 0;
}

//Functions to control the basic functionality of the door seal manager - calls all necessary functions on begin play///////////////////////////////////////////////////////////////
void ADoorSealSpawner::BeginPlay()
{
	Super::BeginPlay();

	GetRoomCount();
	GetRoomPlacementModifier();
	SetupSpawns();
}
void ADoorSealSpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (isArrayUpdated == false)
	{
		if (doorSealArray.Num() > 0)
		{
			for (int i = 0; i < doorSealArray.Num(); i++)
			{
				if (doorSealArray[i]->IsValidLowLevelFast() == false)
				{
					isArrayUpdated = true;
					for (TActorIterator<ADungeonsNDwellingsv4GameMode> ActorItr(GetWorld()); ActorItr; ++ActorItr)
					{
						// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
						ADungeonsNDwellingsv4GameMode *Object = *ActorItr;
						ActorItr->RemoveLoadingScreen();
					}
					break;
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control door seal spawning///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADoorSealSpawner::SetupSpawns()
{
	FVector location;
	FRotator rotation;
	FVector zLocation;

	for (int i = 1; i <= 4; i++)
	{
		for (int j = 1; j <= roomCount; j++)
		{
			zLocation = (roomPlacementModifier * j);
			if (i == 1)
			{
				location = doorPositionsArray[i] + zLocation;
				rotation = doorRotationsArray[i];
			}
			else if (i == 2)
			{
				location = doorPositionsArray[i] + zLocation;
				rotation = doorRotationsArray[i];
			}
			else if (i == 3)
			{
				location = doorPositionsArray[i] + zLocation;
				rotation = doorRotationsArray[i];
			}
			else if (i == 4)
			{
				location = doorPositionsArray[i] + zLocation;
				rotation = doorRotationsArray[i];
			}
			position = FTransform(rotation, location, scale);
			SpawnDoorSeals(position, location, rotation, scale);
		}
	}
}
void ADoorSealSpawner::SpawnDoorSeals(FTransform spawnLocation, FVector loc, FRotator rot, FVector sca)
{
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		ADoorSeal* sealActor = World->SpawnActorDeferred<ADoorSeal>(ADoorSeal::StaticClass(), spawnLocation);
		sealActor->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		sealActor->updateVariables(loc, rot, sca);
		sealActor->FinishSpawning(spawnLocation);
		doorSealArray.Add(sealActor);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to manage modifying or altering door functionality based on player interaction////////////////////////////////////////////////////////////////////////////////////////////
void ADoorSealSpawner::OpenDoors(int roomNumber)
{
	float doorZValue = (roomNumber * roomPlacementModifier.Z) + doorZOffset;
	FVector doorLoc;
	roomsOpened++;

	for (int i = 0; i < doorSealArray.Num(); i++)
	{
		if (doorSealArray[i]->IsValidLowLevel())
		{
			doorLoc = doorSealArray[i]->GetActorLocation();
			if (doorLoc.Z == doorZValue)
			{
				doorSealArray[i]->Destroy();
				doorSealArray.RemoveAt(i);
			}
		}
	}
	if (roomsOpened == roomCount)
	{
		for (int i = 0; i < doorSealArray.Num(); i++)
		{
			if (doorSealArray[i]->IsValidLowLevel())
			{
				doorSealArray[i]->Destroy();
				doorSealArray.RemoveAt(i);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to GET and SET key variables from external classes///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADoorSealSpawner::GetRoomCount()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->GetRoomCount();
	}
}
void ADoorSealSpawner::GetRoomPlacementModifier()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->GetRoomPlacementModifier();
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

