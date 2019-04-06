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

	//Enemy Type 1 Values/////////////////////////////
	const int type1Min = 1;
	const int type1Max = 4;
	//////////////////////////////////////////////////

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int enemySpawnSelector();

	int enemyCountSelector(int enemyType);

	int spawnRoomSelector();

	FTransform getSpawnLocation(int enemyNum, int enemyType);

	void spawnEnemy(int enemyType, int enemyCount, int spawnRoom);

	void activateEnemies(FVector playerLoc);

	void getEnemiesPerRoom();

	void checkRoomCleared(int roomNum);

	void removeArrayItem(FString name);







	UPROPERTY(EditAnywhere)
		int roomCount;

	int enemyTypePicker;

	int spawnChanceSelector;

	int roomToSpawnIn;

	UPROPERTY(EditAnywhere)
		int enemyCount;

	FVector enemyLocation;

	UPROPERTY(EditAnywhere)
		TArray<int32> usedRooms;

	UPROPERTY(EditAnywhere)
		int roomsUsed;

	UPROPERTY(EditAnywhere)
		TArray<ABasicSlugEnemy*> slugEnemyArray;

	UPROPERTY(EditAnywhere)
		TArray<int32> enemiesPerRoom;

	UPROPERTY(EditAnywhere)
		int enemyKilledCounter;

	UPROPERTY(EditAnywhere)
		int enemyMinSpawn;

	UPROPERTY(EditAnywhere)
		int enemyMaxSpawn;

	UPROPERTY(EditAnywhere)
		TArray<FVector> spawnDefaults;
};
