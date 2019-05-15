// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorPathingManager.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API ADoorPathingManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorPathingManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/*******************************************************************************************PRIVATE VARIABLES************************************************************************************/
	
	//Door Modifier + Tracker Variables used for player move algorithms//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		TArray<FVector> doorDefaults;						//default position for doors - holds 4 items
	UPROPERTY(EditAnywhere)
		TArray<int32> arrayOfDoors;							//holds all the doors generated
	UPROPERTY(EditAnywhere)
		TArray<FVector> doorStartPoints;					//sets the doors in appropriate order
	UPROPERTY(EditAnywhere)
		TArray<FVector> doorMappings;						//sets the output and pairings for doors
	UPROPERTY(EditAnywhere)
		TArray<FVector> roomsToAdd;							//array to hold critical path
	UPROPERTY(EditAnywhere)
		TArray<FVector> remainingRooms;						//array to hold all rooms not on critcal path
	UPROPERTY(EditAnywhere)
		TArray<FVector> remainingRooms_One;					//array to split remaining rooms into
	UPROPERTY(EditAnywhere)
		TArray<FVector> remainingRooms_Two;					//array to split remainign rooms into
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Variables to hold values from external classes/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FVector roomPlacementModifier;							//distance between rooms
	int roomCount;											//number of rooms
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


public:	
	/****************************************************************************************PUBLIC FUNCTIONS****************************************************************************************/

	//Door mapping functions/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CreateStartingPointArray();						//create the array to order doors
	void CreateExitPointArray();							//create array to map/pair doors together
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions for getting and passing key variables to other classes///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void CreateArrayOfDoors();								//get doors from generator
	void GetRoomPlacementModifier();						//get distance between doors
	void GetRoomCount();									//get number of rooms
	TArray<FVector> GetDoorMappingArray();					//pass mapping to player
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
