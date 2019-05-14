// Fill out your copyright notice in the Description page of Project Settings.

#include "TileGeneratorParent.h"

// Sets default values
ATileGeneratorParent::ATileGeneratorParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//set up room default values 
	roomXLength = 9;		//default room floor length
	roomYLength = 9;		//default room floor width
	tileSize = 100;			//size of floor/wall tiles
	roomCount = 6;			//default room number
	roomPlacementModifier = FVector(0, 0, 2000); //distance between rooms
	
	//set up wall default values
	wallXLength = 1;			//wall length dimension
	wallYLength = 32;			//wall width dimension
	singleWallLength = (wallYLength / 4);	//length of each wall

	wallNo1 = 1; //these need to be reworked in the future in conjunction with the blueprint for more concise functionality
	wallNo2 = 2; //these are bad
	wallNo3 = 3;

	//set up door default values
	roomDoorCount = 0;	//number of doors defualt
	totalNumDoors = 0;	//total number of doors default
	minNumDoors = 2;	//minimum doors per room - not including last room, last can only have 1 door
	maxNumDoors = 4;	//maximum doors per room


	testingTotal = 0;	//a running total used when setting up doors
	valueToChange = 0;	//default value in array to be modified
}

//Functions to pass blueprint variable values to the other classes as necessary////////////////////////////////////////////////////////////////////////////////////////////////
TArray<int32> ATileGeneratorParent::GetArrayOfDoors()
{
	return (arrayOfDoors); //pass the array that contains all doors spawned
}
int32 ATileGeneratorParent::GetRunningTotal()
{
	return (totalNumDoors); //pass the total number of doors spawned
}
int32 ATileGeneratorParent::GetRoomCount()
{
	return (roomCount);	//pass the total number of rooms
}
FVector ATileGeneratorParent::GetRoomPlacementModifier()
{
	return (roomPlacementModifier);	//pass the distance between rooms
}	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


