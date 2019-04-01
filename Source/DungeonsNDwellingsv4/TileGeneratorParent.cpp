// Fill out your copyright notice in the Description page of Project Settings.

#include "TileGeneratorParent.h"

// Sets default values
ATileGeneratorParent::ATileGeneratorParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	roomXLength = 9;
	roomYLength = 9;
	tileSize = 100;
	roomCount = 5;
	roomPlacementModifier = FVector(0, 0, 2000);


	wallXLength = 1;
	wallYLength = 32;
	singleWallLength = (wallYLength / 4);

	wallNo1 = 1;
	wallNo2 = 2;
	wallNo3 = 3;


	roomDoorCount = 0;
	totalNumDoors = 0;
	minNumDoors = 2;
}

// Called when the game starts or when spawned
void ATileGeneratorParent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATileGeneratorParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*
void ATileGeneratorParent::adjustArrayValues(int total)
{
	testingTotal = 3;
}
*/

TArray<int32> ATileGeneratorParent::getArrayOfDoors()
{
	return (arrayOfDoors);
}

int32 ATileGeneratorParent::getRunningTotal()
{
	return (totalNumDoors);
}

int32 ATileGeneratorParent::getRoomCount()
{
	return (roomCount);
}

FVector ATileGeneratorParent::getRoomPlacementModifier()
{
	return (roomPlacementModifier);
}


