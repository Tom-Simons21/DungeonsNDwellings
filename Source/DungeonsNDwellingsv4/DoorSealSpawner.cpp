// Fill out your copyright notice in the Description page of Project Settings.

#include "DoorSealSpawner.h"
#include "DoorSeal.h"
#include "Engine.h"
#include "Engine/World.h"
#include "TileGeneratorParent.h"
#include "DungeonsNDwellingsv4Pawn.h"

// Sets default values
ADoorSealSpawner::ADoorSealSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	doorSealArray.Empty();
	roomCount = 0;
}

//Functions to control the basic functionality of the door seal manager - calls all necessary functions on begin play///////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
void ADoorSealSpawner::BeginPlay()
{
	Super::BeginPlay();

	roomsOpened = 0;

	GetRoomCount();
	GetRoomPlacementModifier();

	setupSpawns();
}

// Called every frame
void ADoorSealSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control door seal spawning///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADoorSealSpawner::setupSpawns()
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
			spawnDoorSeals(position, location, rotation, scale);
		}
	}
}

void ADoorSealSpawner::spawnDoorSeals(FTransform spawnLocation, FVector loc, FRotator rot, FVector sca)
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
void ADoorSealSpawner::openDoors(int roomNumber)
{
	roomsOpened += 1;

	float doorZValue = (roomNumber * roomPlacementModifier.Z) + doorZOffset;
	FVector doorLoc;

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
		roomCount = ActorItr->getRoomCount();
	}
}

void ADoorSealSpawner::GetRoomPlacementModifier()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->getRoomPlacementModifier();
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

