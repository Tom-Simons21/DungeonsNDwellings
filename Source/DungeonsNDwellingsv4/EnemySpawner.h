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

	//Enemy Spawn Variance Values - These should be preset in here and not accessible in editorm, only to be edited when a new enemy is added//////////////////////////////
	const TArray<int32> minimumSpawnValue = { 0, 1 };
	const TArray<int32> maximumSpawnValue = { 0, 4 };
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	/*************************************************************************PUBLIC VARIABLES***************************************************************************/


	//Variables to identify the randomisation and spawning values for the enemy objects///////////////////////////////////////////////////////////////////////////////////
	int enemyTypePicker;
	int spawnChanceSelector;
	int roomToSpawnIn;
	UPROPERTY(EditAnywhere)
		int enemyCount;
	UPROPERTY(EditAnywhere)
		TArray<int32> usedRooms;
	UPROPERTY(EditAnywhere)
		int roomsUsed;
	UPROPERTY(EditAnywhere)
		int enemyMinSpawn;
	UPROPERTY(EditAnywhere)
		int enemyMaxSpawn;

	UPROPERTY(EditAnywhere)
	float enemyZOffset;
	UPROPERTY(EditAnywhere)
	float playerZOffset;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Arrays of enemy objects - these will hold all spawned enemy objects - one array per enemy type - this allows each enemy object of each type to be modified directly/////
	UPROPERTY(EditAnywhere)
		TArray<ABasicSlugEnemy*> slugEnemyArray;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Variables to track enemies within the current room - checks how many enemies remain/is room complete////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		TArray<int32> enemiesPerRoom;
	UPROPERTY(EditAnywhere)
		int enemyKilledCounter;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Variables to track key values between classes//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		int roomCount;
	FVector roomPlacementModifier;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	/*************************************************************************PUBLIC FUNCTIONS******************************************************************************/

	//Functions to control the random selection of spawn room, generation of enemy per room, sets spawn points and then calls spawn function/////////////////////////////////
	//Currently only acts with slug enemy, should be expandable to include other enemy objects
	int enemySpawnSelector();
	int enemyCountSelector(int enemyType);
	int spawnRoomSelector();
	FTransform getSpawnLocation(int enemyNum, int enemyType);
	void spawnEnemy(int enemyType, int enemyCount, int spawnRoom);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Function to activate enemies or modify any aspect of values or behaviour as is appropriate/////////////////////////////////////////////////////////////////////////////
	void activateEnemies(FVector playerLoc);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions to check current room status and manage enemy objects within a room//////////////////////////////////////////////////////////////////////////////////////////
	void getEnemiesPerRoom();								//checks how many enemies spawned per room
	void checkRoomCleared(int roomNum);						//takes a room number and checks if enemy count == 0
	void removeArrayItem(FString name);						//if an enemy is destroyed this is called, it makes sure destroyed enemies are not left in the array
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions to access and set variables between classes//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void GetRoomCount();
	void GetRoomPlacementModifier();

	void GetPlayerZOffset();
	void GetSlugEnemyZ();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
