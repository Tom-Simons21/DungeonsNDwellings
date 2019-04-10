// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"
#include "BasicSlugEnemy.h"
#include "Engine.h"
#include "Engine/World.h"
#include "TileGeneratorParent.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "DoorSealSpawner.h"
#include "TileGeneratorParent.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ensure all values relative to the enemy manager are reset each time the game is played/////////////////////////
	roomCount = 0; 
	roomPlacementModifier = FVector(0, 0, 0);
	enemyTypePicker = 0;
	enemyCount = 0;
	spawnChanceSelector = 0;
	enemyMinSpawn = 0;
	enemyMaxSpawn = 0;

	//empty arrays
	usedRooms.Empty();
	enemiesPerRoom.Empty();
	slugEnemyArray.Empty();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


//Functions to control basic functionality and on load features of the enemy manager///////////////////////////////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	GetRoomCount();
	GetRoomPlacementModifier();

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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control randomly generating all values respective to enemy spawning, should be expandable to multiple enemy types///////////////////////////////////////////////////////////
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
		enemyMinSpawn = minimumSpawnValue[enemyType];
		enemyMaxSpawn = maximumSpawnValue[enemyType];

		enemyCount = FMath::RandRange(enemyMinSpawn, enemyMaxSpawn);
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
				spawnLocation.AddToTranslation((roomPlacementModifier * spawnRoom));
				spawnLocation.AddToTranslation(FVector(0, 0, 61));
				ABasicSlugEnemy* slugActor = World->SpawnActor<ABasicSlugEnemy>(ABasicSlugEnemy::StaticClass(), spawnLocation);

				slugEnemyArray.Add(slugActor);
			}
		}
	}
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
		zLoc += roomPlacementModifier.Z;
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to modify and affect enemy behaviour and passing any logic or stat functions//////////////////////////////////////////////////////////////////////////////////////////////////////
void AEnemySpawner::activateEnemies(FVector playLoc)
{
	float objectZLoc;

	for (int i = 0; i < slugEnemyArray.Num(); i++)
	{
		objectZLoc = slugEnemyArray[i]->getZLocation();

		if ((objectZLoc - 61) == (playLoc.Z - 22))
		{
			slugEnemyArray[i]->setIsEnemyActive();
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to check room status and manage enemy objects - this will check if rooms have been emptied + remove destroyed enemies from their respective arrays////////////////////////////////
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
	//To modify this in the future without create a function per enemy the name should be checked for a key word "slug" enemies should be appropriately named for this.
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to GET and SET key variables between classes////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AEnemySpawner::GetRoomCount()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->getRoomCount();
	}
}

void AEnemySpawner::GetRoomPlacementModifier()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->getRoomPlacementModifier();
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

