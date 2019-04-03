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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int enemySpawnSelector();

	int enemyCountSelector(int enemyType);

	int spawnRoomSelector();

	FTransform getSpawnLocation(int enemyNum, int enemyType);

	void spawnEnemy(int enemyType, int enemyCount, int spawnRoom);

	void activateEnemies(FVector playerLoc);

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
};
