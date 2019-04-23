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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**********************************************************************PUBLIC VARIABLES******************************************************************************/

	//Door Modifier + Tracker Variables used for player move algorithms///////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		TArray<FVector> doorDefaults;
	UPROPERTY(EditAnywhere)
		TArray<int32> arrayOfDoors;
	UPROPERTY(EditAnywhere)
		TArray<FVector> doorStartPoints;
	UPROPERTY(EditAnywhere)
		TArray<FVector> doorMappings;
	UPROPERTY(EditAnywhere)
		TArray<FVector> roomsToAdd;
	UPROPERTY(EditAnywhere)
		TArray<FVector> remainingRooms;
	UPROPERTY(EditAnywhere)
		TArray<FVector> remainingRooms_One;
	UPROPERTY(EditAnywhere)
		TArray<FVector> remainingRooms_Two;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Variables to hold values from external classes//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FVector roomPlacementModifier;
	int roomCount;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	/*************************************************************************PUBLIC FUNCTIONS***************************************************************************/

	//Door mapping functions//////////////////////////////////////////////////////////////////////////////////////////
	void CreateStartingPointArray();
	void CreateExitPointArray();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions for getting and passing key variables to other classes////////////////////////////////////////////////
	void CreateArrayOfDoors();
	void GetRoomPlacementModifier();
	void GetRoomCount();

	TArray<FVector> GetDoorMappingArray();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
