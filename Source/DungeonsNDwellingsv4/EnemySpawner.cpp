// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"
#include "BasicSlugEnemy.h"
#include "Engine.h"
#include "Engine/World.h"
#include "TileGeneratorParent.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "DoorSealSpawner.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	roomCount = 5;
	enemyTypePicker = 0;
	enemyCount = 0;
	spawnChanceSelector = 0;
	usedRooms.Empty();
	enemiesPerRoom.Empty();
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	roomsUsed = usedRooms.Num();

	enemyKilledCounter = 0;

	
	for (int i = 0; i <= (roomCount - 2); i++)
	{
		if (roomsUsed <= (roomCount - 2))
		{
			enemyTypePicker = enemySpawnSelector();

			enemyCount = enemyCountSelector(enemyTypePicker);

			roomToSpawnIn = spawnRoomSelector();

			spawnEnemy(enemyTypePicker, enemyCount, roomToSpawnIn);
		}
		else
		{
			break;
		}
	}

	getEnemiesPerRoom();
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int AEnemySpawner::enemySpawnSelector()
{
	spawnChanceSelector = FMath::RandRange(1, 100);

	if (spawnChanceSelector >= 1 && spawnChanceSelector <= 20)
	{
		enemyTypePicker = 0;
	}
	else if (spawnChanceSelector > 20 && spawnChanceSelector <= 100)
	{
		enemyTypePicker = 1;
	}

	return (enemyTypePicker);
}

int AEnemySpawner::enemyCountSelector(int enemyType)
{
	if (enemyType == 0)
	{
		enemyCount = 0;
	}
	if (enemyType == 1)
	{
		enemyCount = FMath::RandRange(2, 4);
	}

	return (enemyCount);
}

int AEnemySpawner::spawnRoomSelector()
{
	bool isRoomFree;
	roomsUsed = usedRooms.Num();
	FString roomOutput;
	
	do
	{
		isRoomFree = true;
		roomToSpawnIn = FMath::RandRange(1, (roomCount - 1));

		if (roomsUsed == 0)
		{
			break;
		}
		else if (roomsUsed == (roomCount - 1))
		{
			break;
		}

		for (int i = 0; i < roomsUsed; i++)
		{
			if (usedRooms[i] == roomToSpawnIn)
			{
				isRoomFree = false;
				break;
			}
		}

	} while (isRoomFree == false);
	
	usedRooms.Add(roomToSpawnIn);
	return (roomToSpawnIn);

}

void AEnemySpawner::spawnEnemy(int enemyType, int enemyCount, int spawnRoom)
{
	FString roomSelected;
	FTransform spawnLocation;

	roomSelected = FString::FromInt(spawnRoom);

	UWorld* const World = GetWorld();
	if (enemyType == 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("No enemy has spawned in: %s"), *roomSelected));
		}

		for (TActorIterator<ADoorSealSpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADoorSealSpawner *Object = *ActorItr;
			ActorItr->openDoors(spawnRoom);
		}
	}
	else if (enemyType == 1)
	{
		if (World != NULL)
		{
			for (int i = 1; i <= enemyCount; i++)
			{
				spawnLocation = getSpawnLocation(i, enemyType);
				spawnLocation.AddToTranslation((FVector(0, 0, 2000) * spawnRoom));
				spawnLocation.AddToTranslation(FVector(0, 0, 61));
				ABasicSlugEnemy* slugActor = World->SpawnActor<ABasicSlugEnemy>(ABasicSlugEnemy::StaticClass(), spawnLocation);

				slugEnemyArray.Add(slugActor);
			}
		}
	}
	
	
}


FTransform AEnemySpawner::getSpawnLocation(int enemyNum, int enemyType)
{
	FVector loc;
	FRotator rot = FRotator(0, 0, 0);
	FVector scale = FVector(0.4, 0.4, 1.2);
	FTransform enemyPosition;

	if (enemyType == 1)
	{
		if (enemyNum == 1)
		{
			loc = FVector(100, 100, 0);
		}
		else if (enemyNum == 2)
		{
			loc = FVector(100, 700, 0);
		}
		else if (enemyNum == 3)
		{
			loc = FVector(700, 700, 0);
		}
		else if (enemyNum == 4)
		{
			loc = FVector(700, 100, 0);
		}
	}
	else
	{
		loc = FVector(0, 0, 0);
		rot = FRotator(0, 0, 0);
		scale = FVector(0, 0, 0);
	}

	enemyPosition = FTransform(rot, loc, scale);

	return (enemyPosition);
}


void AEnemySpawner::getEnemiesPerRoom()
{
	int enemyCounter;
	float zLoc = 61;
	float enemyZLoc;

	for (int i = 0; i <= roomCount; i++)
	{
		enemyCounter = 0;

		for (int j = 0; j < slugEnemyArray.Num(); j++)
		{
			enemyZLoc = slugEnemyArray[j]->getZLocation();

			if (enemyZLoc == zLoc)
			{
				enemyCounter++;
			}
		}

		enemiesPerRoom.Add(enemyCounter);
		zLoc += 2000;
	}

}


void AEnemySpawner::activateEnemies(FVector playLoc)
{
	float objectZLoc;

	if (slugEnemyArray.Num() != 0)
	{
		for (int i = 0; i < slugEnemyArray.Num(); i++)
		{
			objectZLoc = slugEnemyArray[i]->getZLocation();

			if ((objectZLoc - 61) == (playLoc.Z - 22))
			{
				slugEnemyArray[i]->setIsEnemyActive();
			}

		}
	}
}


void AEnemySpawner::checkRoomCleared(int roomNum)
{
	int enemiesInRoom;
	int roomVal = roomNum;

	enemiesInRoom = enemiesPerRoom[roomNum];

	enemyKilledCounter++;

	if (enemyKilledCounter == enemiesInRoom)
	{
		enemyKilledCounter = 0;
		
		for (TActorIterator<ADoorSealSpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADoorSealSpawner *Object = *ActorItr;
			ActorItr->openDoors(roomVal);
		}
		
	}
}

void AEnemySpawner::removeArrayItem(FString objName)
{
	FString enemyName;

	for (int i = 0; i < slugEnemyArray.Num(); i++)
	{
		enemyName = slugEnemyArray[i]->GetName();

		if (enemyName == objName)
		{
			slugEnemyArray.RemoveAt(i);
		}
	}
}


void AEnemySpawner::setPlayerDamage(float dmg)
{
	playerCurrentDmg = dmg;
}

float AEnemySpawner::getPlayerCurrentDmg()
{
	return (playerCurrentDmg);
}

