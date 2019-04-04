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

	roomCount = 0;
}

// Called when the game starts or when spawned
void ADoorSealSpawner::BeginPlay()
{
	Super::BeginPlay();

	setupSpawns();
}

// Called every frame
void ADoorSealSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//checkDoorStatus();
}

void ADoorSealSpawner::setupSpawns()
{
	FVector location;
	FRotator rotation;
	FVector scale;
	FVector zLocation;
	roomCount = getRoomCount();

	for (int i = 1; i <= 4; i++)
	{
		for (int j = 1; j <= roomCount; j++)
		{
			zLocation = (FVector(0, 0, 2000) * j);

			if (i == 1)
			{
				location = FVector(20, 400, 61) + zLocation;
				rotation = FRotator(0, 0, 0);
				scale = FVector(0.3, 0.95, 1.2);
			}
			else if (i == 2)
			{
				location = FVector(400, 780, 61) + zLocation;
				rotation = FRotator(0, 90, 0);
				scale = FVector(0.3, 0.95, 1.2);
			}
			else if (i == 3)
			{
				location = FVector(780, 400, 61) + zLocation;
				rotation = FRotator(0, 0, 0);
				scale = FVector(0.3, 0.95, 1.2);
			}
			else if (i == 4)
			{
				location = FVector(400, 20, 61) + zLocation;
				rotation = FRotator(0, 90, 0);
				scale = FVector(0.3, 0.95, 1.2);
			}

			position = FTransform(rotation, location, scale);
			spawnDoorSeals(position, location, rotation, scale);
		}
	}
}

void ADoorSealSpawner::spawnDoorSeals(FTransform spawnLocation, FVector loc, FRotator rot, FVector sca)
{
	//FActorSpawnParameters spawnInfo;
	//spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

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


int ADoorSealSpawner::getRoomCount()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->getRoomCount();
	}

	return (roomCount);
}


void ADoorSealSpawner::openDoors(int roomNumber)
{
	float doorZValue = (roomNumber * 2000) + 61;
	FVector doorLoc;
	float doorOpenOffset = 80;

	for (int i = 0; i < doorSealArray.Num(); i++)
	{
		doorLoc = doorSealArray[i]->GetActorLocation();

		if (doorLoc.Z == doorZValue)
		{
			doorSealArray[i]->Destroy();
			doorSealArray.RemoveAt(i);
		}
	}
}
