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
	roomCount = 6;
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
	maxNumDoors = 4;


	testingTotal = 0;
	valueToChange = 0;
}

//Functions to pass blueprint variable values to the other classes as necessary////////////////////////////////////////////////////////////////////////////////////////////////
TArray<int32> ATileGeneratorParent::GetArrayOfDoors()
{
	return (arrayOfDoors);
}
int32 ATileGeneratorParent::GetRunningTotal()
{
	return (totalNumDoors);
}
int32 ATileGeneratorParent::GetRoomCount()
{
	return (roomCount);
}
FVector ATileGeneratorParent::GetRoomPlacementModifier()
{
	return (roomPlacementModifier);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


