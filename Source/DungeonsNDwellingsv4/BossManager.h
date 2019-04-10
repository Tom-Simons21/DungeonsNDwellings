// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossManager.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API ABossManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*******************************************************************************PUBLIC VARIABLES***************************************************************************/

	//Projectile Boss Spawn Point//////////////////////////////////////////////////////////////////////////////////////
	FVector const spawnLocation = FVector(400, 400, 10032);								  	
	FRotator const spawnRotation = FRotator(0, 0, 0);									  
	FVector const spawnScale = FVector(1.3, 1.3, 0.6);									  
	FTransform const spawnTrans = FTransform(spawnRotation, spawnLocation, spawnScale);   
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//These values will be constant across all classes/////////////////////////////////////////////////////////////////
	float playerOffset;																	  
	float roomPlacementModifier;
	int roomCount;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	//Constant Z Offset for this boss//////////////////////////////////////////////////////////////////////////////////
	float const projectileBossOffset = 32;												  
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	//Variables to manage boss items and the relevant attached features////////////////////////////////////////////////
	bool isBossKilled;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	






	/**************************************************************************PUBLIC FUNCTIONS***************************************************************************/

	//Functions to retrieve key variables from other classes go here///////////////////////////////////////////////////
	void GetRoomPlacementModifier();																 
	void GetPlayerZOffset();																		 
	void GetRoomCount();
	float GetZOffset();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Boss behaviour and gameplay functions go here////////////////////////////////////////////////////////////////////
	void ActivateBoss(FVector playerLoc);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions to manipulate external features and gameplay mechanics based on boss interaction///////////////////////
	void SetIsBossKilled();
	void AddLevelCompleteElements();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
