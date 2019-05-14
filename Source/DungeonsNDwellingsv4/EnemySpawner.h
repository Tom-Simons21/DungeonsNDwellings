// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BasicSlugEnemy.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Enemy Spawn Variance Values - These should be preset in here and not accessible in editorm, only to be edited when a new enemy is added//////////////////////////////////
	const TArray<int32> minimumSpawnValue = { 0, 1 };		//spawn amount minimums for various enemy types
	const TArray<int32> maximumSpawnValue = { 0, 4 };		//spawn amount maximums for various enemy types
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


private:
	/*************************************************************************PRIVATE VARIABLES********************************************************************************/

	//Variables to identify the randomisation and spawning values for the enemy objects////////////////////////////////////////////////////////////////////////////////////////
	int enemyTypePicker;				//type of enemy to spawn
	int spawnChanceSelector;			//chance to spawn
	int roomToSpawnIn;					//number of room to be spawned in
	int roomsUsed;						//number of occupied rooms
	int enemyMinSpawn;					//holds min spawn for chosen enemy type
	int enemyMaxSpawn;					//holds max spawn for thosen enemy type
	float enemyZOffset;					//elevation off ground
	float playerZOffset;				//player off ground elevation
	int enemyCount;						//number of enemies per room
	TArray<int32> usedRooms;			//array of used rooms
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Arrays of enemy objects - these will hold all spawned enemy objects - one array per enemy type - this allows each enemy object of each type to be modified directly/////
	UPROPERTY(EditAnywhere)
		TArray<ABasicSlugEnemy*> slugEnemyArray;		//array of slug enemies
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Variables to track enemies within the current room - checks how many enemies remain/is room complete////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		TArray<int32> enemiesPerRoom;			//array of enemies per room
	UPROPERTY(EditAnywhere)
		int enemyKilledCounter;					//current killed enemies
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Variables to control external features attached to enemies//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float moneyDropChance;						//chance to drop currency
	float healthDropChance;						//chance to drop health
	FRotator moneyRotation = FRotator(0, 0, 0);		//these do nothing so not going to comment further
	FVector moneyScale = FVector(0.3, 0.3, 0.3);
	FRotator healthRotation = FRotator(0, 0, 0);
	FVector healthScale = FVector(0.3, 0.3, 0.3);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Variables to track key values between classes///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int roomCount;						//count of rooms
	FVector roomPlacementModifier;		//distance between rooms
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



public:	
	/*************************************************************************PUBLIC FUNCTIONS******************************************************************************/

	//Functions to control the random selection of spawn room, generation of enemy per room, sets spawn points and then calls spawn function/////////////////////////////////
	//Currently only acts with slug enemy, should be expandable to include other enemy objects
	int EnemySpawnSelector();								//select which enemy should spawn
	int EnemyCountSelector(int enemyType);					//how many should spawn
	int SpawnRoomSelector();								//which room should they spawn in
	FTransform GetSpawnLocation(int enemyNum, int enemyType);		//sets the spawn location based on which enemy is being spawned in that room
	void SpawnEnemy(int enemyType, int enemyCount, int spawnRoom);		//spawns the enemy
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Function to activate enemies or modify any aspect of values or behaviour as is appropriate/////////////////////////////////////////////////////////////////////////////
	void ActivateEnemies(FVector playerLoc);				//turns enemies on uwu
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions to check current room status and manage enemy objects within a room//////////////////////////////////////////////////////////////////////////////////////////
	void GetEnemiesPerRoom();								//checks how many enemies spawned per room
	void CheckRoomCleared(int roomNum);						//takes a room number and checks if enemy count == 0
	void RemoveArrayItem(FString name);						//if an enemy is destroyed this is called, it makes sure destroyed enemies are not left in the array
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions to control external features of all enemies//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void GainHealthOnKill();						//adds health to player when enemy dies
	void DropMoneyOnDeath(FVector enemyLoc);			//drops money - but doesn't
	void DropHealthOnDeath(FVector enemyLoc);			//drops health - but doesn't
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions to access and set variables between classes//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void GetRoomCount();						//gets room number
	void GetRoomPlacementModifier();			//gets distance between rooms
	void GetPlayerZOffset();						//gets player elevation
	void GetSlugEnemyZ();								//gets slug elevation
	void SetMoneyDropChance(float dropChanceModifier);		//set drop chance
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
