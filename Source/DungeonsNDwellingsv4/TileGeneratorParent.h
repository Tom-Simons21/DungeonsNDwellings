// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileGeneratorParent.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API ATileGeneratorParent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileGeneratorParent();

public:	
	/****************************************************************************PUBLIC VARIABLES****************************************************************************/

	//Variables to hold all the defaults used in the level/world building algorithm, initialised + declared here so they are exposed to other classes////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Properties")
		int32 roomXLength;								//length of floor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Properties")
		int32 roomYLength;								//width of floor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Properties")
		int32 tileSize;									//size of floor/wall tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Properties")
		int32 roomCount;								//number of rooms 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Properties")
		FVector roomPlacementModifier;					//distance between rooms
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
		int32 wallXLength;								//length of wall
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
		int32 wallYLength;								//width of wall
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
		int32 singleWallLength;							//number of tiles in a wall
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
		int32 wallNo1;									//these are bad
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
		int32 wallNo2;									// " "
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
		int32 wallNo3;									// " "
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
		int32 roomDoorCount;							//doors per room
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
		TArray<int32> arrayOfDoors;						//array to hold count of doors in each room
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
		int32 totalNumDoors;							//total number of doors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
		int32 minNumDoors;								//minimum number of doors per room
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
		int32 maxNumDoors;								//maximum number of doors per room
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
		int32 testingTotal;								//a running total of doors used during spawning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
		bool isCompleted;								//bool to check if a valid door system has been developed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
		int32 valueToChange;							//door number to be changed, this will reduce/increase number of doors on a particular floor to ensure the door count is between the min total and max total
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
		int32 minTotalDoors;							//minimum total number of doors, total must be above a certain value to make logical sense
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
		int32 maxTotalDoors;							//maximum total number of doors, total must be below a certain value to make logical sense
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	/***********************************************************************************PUBLIC FUNCTIONS****************************************************************************/

	//Functions to GET values from blueprint, used by other functions to access blueprint variables (READ ONLY)/////////////////////////////////////////////////////////////////////
	TArray<int32> GetArrayOfDoors();		//pass array of doors spawned
	int32 GetRunningTotal();				//get number of doors spawned
	int32 GetRoomCount();					//get number of rooms
	FVector GetRoomPlacementModifier();		//get distance between rooms
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
