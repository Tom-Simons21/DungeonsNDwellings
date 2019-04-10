// Fill out your copyright notice in the Description page of Project Settings.

#include "TileGeneratorParent.h"

// Sets default values
ATileGeneratorParent::ATileGeneratorParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//set up room default values 
	roomXLength = 9;
	roomYLength = 9;
	tileSize = 100;
	roomCount = 5;
	roomPlacementModifier = FVector(0, 0, 2000);
	
	//set up wall default values
	wallXLength = 1;
	wallYLength = 32;
	singleWallLength = (wallYLength / 4);

	wallNo1 = 1; //these need to be reworked in the future in conjunction with the blueprint for more concise functionality
	wallNo2 = 2;
	wallNo3 = 3;

	//set up door default values
	roomDoorCount = 0;
	totalNumDoors = 0;
	minNumDoors = 2;
}

//Standard begin play and tick functions, unlikely these should ever be used as this class only exists to expose blueprint variables to other classes//////////////////////////
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to pass blueprint variable values to the other classes as necessary////////////////////////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


