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
	roomCount = 0;										//defaults
	roomPlacementModifier = FVector(0, 0, 0);			//all values should be assigned appropriately, no risk of uninitialised variables
	enemyTypePicker = 0;
	enemyCount = 0;
	spawnChanceSelector = 0;
	enemyMinSpawn = 0;
	enemyMaxSpawn = 0;

	//empty arrays
	usedRooms.Empty();					//array of popultated rooms
	enemiesPerRoom.Empty();				//array of enemies per rooms
	slugEnemyArray.Empty();				//array of slug enemies
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//drop on death values
	moneyDropChance = 1;
	healthDropChance = 2;
}

//Functions to control basic functionality and on load features of the enemy manager//////////////////////////////////////////////////////////////////////////////////////////////////////////
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	GetRoomCount();						//gets number of rooms
	GetRoomPlacementModifier();			//distance between rooms
	GetPlayerZOffset();					//player Z location

	roomsUsed = usedRooms.Num();		//how many rooms are populated, here it should be 0
	enemyKilledCounter = 0;				//no enemies are dead on level start
	//enemy spawner always uses roomCount - 2 to account for no spawning in first or last room
	for (int i = 0; i <= (roomCount - 2); i++)	//can spawn an enemy in all rooms except for 1 and last
	{
		if (roomsUsed <= (roomCount - 2))		//second check should never be false
		{
			enemyTypePicker = EnemySpawnSelector();					//pick which type of enemy will spawn currently - 0 = no spawn, 1 = slug
			enemyCount = EnemyCountSelector(enemyTypePicker);		//number of enemies to spawn this will be based on type 
			roomToSpawnIn = SpawnRoomSelector();					//room to spawn in
			SpawnEnemy(enemyTypePicker, enemyCount, roomToSpawnIn);	//use all above conditions to dictate spawning
		}
		else
		{
			break;
		}
	}
	GetSlugEnemyZ();		//get Z offset for slugs, only works after 1 is spawned
	GetEnemiesPerRoom();	//track how many enemies per room
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control randomly generating all values respective to enemy spawning, should be expandable to multiple enemy types//////////////////////////////////////////////////////////////
int AEnemySpawner::EnemySpawnSelector()
{
	spawnChanceSelector = FMath::RandRange(1, 100);	//chance to spawn

	if (spawnChanceSelector >= 1 && spawnChanceSelector <= 20)	//20% chance for empty room
	{
		enemyTypePicker = 0;
	}
	else if (spawnChanceSelector > 20 && spawnChanceSelector <= 100) //otherwise spawn slug
	{
		enemyTypePicker = 1;
	}
	return (enemyTypePicker);	//return the type selected on this iteration
}
int AEnemySpawner::EnemyCountSelector(int enemyType)
{
	if (enemyType == 0)
	{
		enemyCount = 0; //if type is 0, nothing is spawning
	}
	if (enemyType == 1)
	{
		enemyMinSpawn = minimumSpawnValue[enemyType];					//enemy spawn parameters should be in array where there type matches the parameter position
		enemyMaxSpawn = maximumSpawnValue[enemyType];
		enemyCount = FMath::RandRange(enemyMinSpawn, enemyMaxSpawn);	//get random value between range
	}
	return (enemyCount);	//return the count
}
int AEnemySpawner::SpawnRoomSelector()
{
	bool isRoomFree;				//first check a room is empty
	roomsUsed = usedRooms.Num();	//check number rooms used
	do
	{
		isRoomFree = true;			//assume room is free
		roomToSpawnIn = FMath::RandRange(1, (roomCount - 1));  //pick a room
		if (roomsUsed == 0)		//if no rooms are used then we dont need to check
		{
			break;
		}
		else if (roomsUsed == (roomCount - 1))	//if all rooms are used break
		{
			break;
		}
		for (int i = 0; i < roomsUsed; i++)   //else check if the room we selected is used
		{
			if (usedRooms[i] == roomToSpawnIn)
			{
				isRoomFree = false; //if yes restart do while
				break;
			}
		}
	} while (isRoomFree == false);
	usedRooms.Add(roomToSpawnIn);	//otherwise add room to used rooms
	return (roomToSpawnIn);			//return the selected free room
}
FTransform AEnemySpawner::GetSpawnLocation(int enemyNum, int enemyType)
{
	FVector loc;								//spawn location
	FRotator rot = FRotator(0, 0, 0);			//spawn rotation
	FVector scale = FVector(0.4, 0.4, 1.2);		//spawn scale
	FTransform enemyPosition;					//setup transform

	if (enemyType == 1)
	{
		if (enemyNum == 1)
		{
			loc = FVector(100, 100, 0);		//these are the spawn points for the slugs, need to be moved to an array in the future
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
		loc = FVector(0, 0, 0);					//else only = 0 right now so spawn points are irrelevant
		rot = FRotator(0, 0, 0);
		scale = FVector(0, 0, 0);
	}
	enemyPosition = FTransform(rot, loc, scale);	//new transform
	return (enemyPosition);							//return spawn location
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function to spawn respective enemies////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AEnemySpawner::SpawnEnemy(int enemyType, int enemyCount, int spawnRoom)
{
	FTransform spawnLocation; //spawn loc

	UWorld* const World = GetWorld();
	if (enemyType == 0)
	{
		//if (GEngine)
		//{
		//	 GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("No enemy has spawned in: %s"), *roomSelected));
		//}
		for (TActorIterator<ADoorSealSpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADoorSealSpawner *Object = *ActorItr;
			ActorItr->OpenDoors(spawnRoom);		//if the enemy type for this room is 0, i.e. no enemies open door
		}
	}
	else if (enemyType == 1) // if it is slug enemy
	{
		if (World != NULL)
		{
			for (int i = 1; i <= enemyCount; i++) //loop for chosen count
			{
				spawnLocation = GetSpawnLocation(i, enemyType);									//get the spawn location
				spawnLocation.AddToTranslation((roomPlacementModifier * spawnRoom));			//adjust based on the room being spawned in
				spawnLocation.AddToTranslation(FVector(0, 0, 61));					//add slug Z adjust, needs to be swapped off hard coded magic number - 61 = ZOffset - can't call from actor though before it has spawned ????
				ABasicSlugEnemy* slugActor = World->SpawnActor<ABasicSlugEnemy>(ABasicSlugEnemy::StaticClass(), spawnLocation); //spawn
				slugEnemyArray.Add(slugActor); //add to array of actors
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Track how many enemies have been spawned in each room - then we know how many to kill to clear each room////////////////////////////////////////////////////////////////////////////////////
void AEnemySpawner::GetEnemiesPerRoom()
{
	int enemyCounter;						//number of enemies
	float zLoc;								//distance enemy is off ground
	int enemyRoomOccupying;

	for (int i = 0; i <= roomCount; i++) //count per room
	{
		enemyCounter = 0; //each room starts at 0
		for (int j = 0; j < slugEnemyArray.Num(); j++)
		{
			zLoc = slugEnemyArray[j]->GetZLocation();
			enemyRoomOccupying = FMath::FloorToInt(zLoc / roomPlacementModifier.Z);	//enemy occupy room = Z / distance between rooms i.e. room 2 would be at 4000 - then truncate to remove offset
			if (enemyRoomOccupying == i)	//get the room number check every enemy that has a value implying they occupy that room
			{
				enemyCounter++;	//therefore counter + 1
			}
		}
		enemiesPerRoom.Add(enemyCounter); //adjust array to track items
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to modify and affect enemy behaviour and passing any logic or stat functions//////////////////////////////////////////////////////////////////////////////////////////////////////
void AEnemySpawner::ActivateEnemies(FVector playLoc)
{
	float objectZLoc;			//enemy Z coord
	int room;					//room number
	int playerRoom;				//room player is in
	for (int i = 0; i < slugEnemyArray.Num(); i++)
	{
		objectZLoc = slugEnemyArray[i]->GetZLocation();							//get location of respective enemy object
		room = FMath::FloorToInt(objectZLoc / roomPlacementModifier.Z);			//calculate room enemy is in there coord / 2000, truncate to ignore offset, this needs to be implemented across the project honestly
		playerRoom = FMath::FloorToInt(playLoc.Z / roomPlacementModifier.Z);	//do the same for the player

		if (room == playerRoom) //compare rooms numbers
		{
			UE_LOG(LogTemp, Warning, TEXT("Value is: %d"), playerRoom); //debug message
			slugEnemyArray[i]->SetIsEnemyActive();	//active relevant enemies
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to check room status and manage enemy objects - this will check if rooms have been emptied + remove destroyed enemies from their respective arrays////////////////////////////////
void AEnemySpawner::CheckRoomCleared(int roomNum)
{
	int enemiesInRoom;								//number of enemies in a room
	int roomVal = roomNum;							//variable to hold incoming variable, kind of unnecessary but personal preference

	enemiesInRoom = enemiesPerRoom[roomNum];		//check array for enemy count in room
	enemyKilledCounter++;							//everytime called increment killed counter

	if (enemyKilledCounter == enemiesInRoom)		//when killed counter == enemies in the room reset counter open doors
	{
		enemyKilledCounter = 0;						//reset
		for (TActorIterator<ADoorSealSpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADoorSealSpawner *Object = *ActorItr;
			ActorItr->OpenDoors(roomVal);			//open doors
		}
	}
}
void AEnemySpawner::RemoveArrayItem(FString objName)	//remove enemy from array
{
	FString enemyName; //To modify this in the future without create a function per enemy the name should be checked for a key word "slug" enemies should be appropriately named for this.

	GainHealthOnKill(); //when an enemy dies we are going to check if the player has the sacrifice bonus
	for (int i = 0; i < slugEnemyArray.Num(); i++)
	{
		enemyName = slugEnemyArray[i]->GetName();	//find the enemy by name in array
		if (enemyName == objName)
		{
			//effects to happen on enemy death
			DropMoneyOnDeath(slugEnemyArray[i]->GetActorLocation());		//apply death effects, currently nothing
			DropHealthOnDeath(slugEnemyArray[i]->GetActorLocation());
			slugEnemyArray.RemoveAt(i);										//remove
		}
	}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control external features of all enemies/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AEnemySpawner::GainHealthOnKill()
{
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->GainHealthOnKill();					//player gets health when they kill enemy
	}
}
void AEnemySpawner::DropMoneyOnDeath(FVector loc)	//chance for money to drop but currently unfinished
{
	float chanceToDrop = FMath::RandRange(1, 100);
	FTransform dropLocation;

	if (chanceToDrop <= moneyDropChance)
	{
		//if (GEngine)
		//{
		//	  GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Money Dropped")));
		//}
	}
}
void AEnemySpawner::DropHealthOnDeath(FVector loc)		//chance for health to drop but currently unfinished
{
	float chanceToDrop = FMath::RandRange(1, 100);
	FTransform dropLocation;

	if (chanceToDrop <= healthDropChance)
	{
		//if (GEngine)
		//{
		//    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Health Dropped")));
		//}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to GET and SET key variables between classes////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AEnemySpawner::GetRoomCount()				//get number of rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->GetRoomCount();
	}
}
void AEnemySpawner::GetRoomPlacementModifier()   //get distance between rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->GetRoomPlacementModifier();
	}
}
void AEnemySpawner::GetPlayerZOffset()   //get player elevation off of ground
{
	FVector zOffset;
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		zOffset = ActorItr->GetPlayerZOffset();
	}
	playerZOffset = zOffset.Z;
}
void AEnemySpawner::GetSlugEnemyZ()			//get slug elevation off of ground
{
	for (TActorIterator<ABasicSlugEnemy> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ABasicSlugEnemy *Object = *ActorItr;
		enemyZOffset = ActorItr->GetZOffset();
	}
}
void AEnemySpawner::SetMoneyDropChance(float dropChanceModifier) //set drop chances - does nothing except modifies an invisible value right now
{
	moneyDropChance = moneyDropChance * dropChanceModifier;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

