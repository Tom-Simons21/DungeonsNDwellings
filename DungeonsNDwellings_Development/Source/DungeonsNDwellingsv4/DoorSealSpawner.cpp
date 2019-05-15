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
	SetTickableWhenPaused(true);	//doors will be set during a game pause / loading screen at start

	doorSealArray.Empty();			//array of door seals
	roomCount = 0;					//number of rooms
	roomsOpened = 0;				//number of rooms cleared
}

//Functions to control the basic functionality of the door seal manager - calls all necessary functions on begin play///////////////////////////////////////////////////////////////
void ADoorSealSpawner::BeginPlay()
{
	Super::BeginPlay();

	GetRoomCount();						//gets number of rooms
	GetRoomPlacementModifier();			//gets distance between rooms
	SetupSpawns();						//sets spawns
}
void ADoorSealSpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (isArrayUpdated == false)			//checks if array is done populating
	{
		if (doorSealArray.Num() > 0)		//checks the array has values in it
		{
			for (int i = 0; i < doorSealArray.Num(); i++)	//check every entry
			{
				if (doorSealArray[i]->IsValidLowLevelFast() == false)	//check if a value == null
				{
					isArrayUpdated = true;	//as soon as a value == null the array has finished populating
					for (TActorIterator<ADungeonsNDwellingsv4GameMode> ActorItr(GetWorld()); ActorItr; ++ActorItr)
					{
						// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
						ADungeonsNDwellingsv4GameMode *Object = *ActorItr;
						ActorItr->RemoveLoadingScreen(); //when array has finished populating remove the loading screen / play game
					}
					break; //break when done
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control door seal spawning///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADoorSealSpawner::SetupSpawns()
{
	FVector location;			//door spawn location
	FRotator rotation;			//door spawn rotation
	FVector zLocation;			//room number dictated by Z height

	for (int i = 1; i <= 4; i++)	//loop 4 times, rooms can have 4 doors
	{
		for (int j = 1; j <= roomCount; j++) //loop for each room
		{
			zLocation = (roomPlacementModifier * j); //calculate Z height
			if (i == 1)	//this does not need to be here, this needs changing nothing is being checked for
			{			//will be removed in post most likely due to an irrational fear of breaking things via making logical changes
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
			position = FTransform(rotation, location, scale);		//set the position of the door
			SpawnDoorSeals(position, location, rotation, scale);	//spawn the door
		}
	}
}
void ADoorSealSpawner::SpawnDoorSeals(FTransform spawnLocation, FVector loc, FRotator rot, FVector sca)
{
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		ADoorSeal* sealActor = World->SpawnActorDeferred<ADoorSeal>(ADoorSeal::StaticClass(), spawnLocation);	//spawn deferred so to modify variables before spawning
		sealActor->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;		//do not spawn on collsion, this happens in post which is why loading screens are used
		sealActor->updateVariables(loc, rot, sca);	//update positions
		sealActor->FinishSpawning(spawnLocation);	//finish the spawn
		doorSealArray.Add(sealActor);				//add the seal to the array
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to manage modifying or altering door functionality based on player interaction////////////////////////////////////////////////////////////////////////////////////////////
void ADoorSealSpawner::OpenDoors(int roomNumber)				//open the doors when a room has no enemies in it
{
	float doorZValue = (roomNumber * roomPlacementModifier.Z) + doorZOffset;	//get the Z value of a door
	FVector doorLoc;				//get the location of the door
	roomsOpened++;					//increment rooms opened every time function is called

	for (int i = 0; i < doorSealArray.Num(); i++)	//remove all doors in the room passed
	{
		if (doorSealArray[i]->IsValidLowLevel())	//check that a valid object is being accessed
		{
			doorLoc = doorSealArray[i]->GetActorLocation();	//get the location of object
			if (doorLoc.Z == doorZValue)					//if door has the correct Z value for the room passed
			{
				doorSealArray[i]->Destroy();				//destroy the object from array
				doorSealArray.RemoveAt(i);					//remove the object from array
			}
		}
	}
	if (roomsOpened == roomCount)				//check how many rooms opened
	{
		for (int i = 0; i < doorSealArray.Num(); i++)	//go through all remaining doors
		{												//this is to ensure on level full clear all doors are gone, a failsafe should not be needed 
			if (doorSealArray[i]->IsValidLowLevel())	//but was added prior during a sequence of bug fixes that were eventually solved by the loading screen system
			{
				doorSealArray[i]->Destroy();			//destroy any valid object left in array
				doorSealArray.RemoveAt(i);				//remove any valid object
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to GET and SET key variables from external classes///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADoorSealSpawner::GetRoomCount()			//get number of rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->GetRoomCount();
	}
}
void ADoorSealSpawner::GetRoomPlacementModifier()		//get distance between rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->GetRoomPlacementModifier();
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

