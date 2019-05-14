// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorSeal.h"
#include "DoorSealSpawner.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API ADoorSealSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorSealSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	/**********************************************************************PRIVATE VARIABLES***************************************************************************/

	bool isArrayUpdated = false;			//check if array has finished populating

	//Variables to manage door spawning and management/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FTransform position;								//location to spawn door
	FVector const scale = FVector(0.3, 0.95, 1.2);		//doors have constant scale but are rotated + located variably
	float const doorZOffset = 61;						//door distance from floor
	UPROPERTY(EditAnywhere)
		TArray<ADoorSeal*> doorSealArray;				//array of door seals
	TArray<FVector> doorPositionsArray = { FVector(0, 0, 0), FVector(20, 400, 61), FVector(400, 780, 61), FVector(780, 400, 61), FVector(400, 20, 61) };	//default door positions
	TArray<FRotator> doorRotationsArray = { FRotator(0, 0, 0), FRotator(0, 0, 0), FRotator(0, 90, 0), FRotator(0, 0, 0), FRotator(0, 90, 0) };	//default door rotations
	UPROPERTY(EditAnywhere)
		int roomsOpened;		//number of rooms opened
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Variables to GET and/or SET as necessary to external classes/////////////////////////////////////////////////////////////////////////////////////////////////////
	int roomCount;						//room count
	FVector roomPlacementModifier;		//distance between rooms
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:	
	/**********************************************************************PUBLIC FUNCTIONS***************************************************************************/

	//Functions to spawn and manage door placement/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SetupSpawns();				//sets the spawn positions
	void SpawnDoorSeals(FTransform spawnLocation, FVector loc, FRotator rot, FVector scale); //sp[awns doors at positions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions to open/modify doors in relation to player activities//////////////////////////////////////////////////////////////////////////////////////////////////
	void OpenDoors(int roomNum);			//opens doors when rooms are cleared
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions to GET and SET key variables between classes///////////////////////////////////////////////////////////////////////////////////////////////////////////
	void GetRoomCount();									//get number of rooms
	void GetRoomPlacementModifier();						//get distance between rooms
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
