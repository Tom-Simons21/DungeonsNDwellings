// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableObject.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API AInteractableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Interactable object component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InteractableObject, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ConeMesh;


	/****************************************************************************PUBLIC VARIABLES***************************************************************************/

	//Variables to control interactable item spawning////////////////////////////////////////////////////////////////////////////////////////////////////////
	int Iteration;												//integer variable to dictate how many times we "spawn" our interactable item
	float TimeToSpawn;											//spawns object whenever Iteration is true
	FVector const startingSpawnPoint = FVector(400, 400, 50);
	FVector secondSpawnPoint;
	int roomCount;
	FVector placementMod;
	bool isLevelComplete;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Variables to control player interaction with object////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
	float distanceFromPlayer;						//this will track how far away the player is from the interactable
	bool isItemRerolled;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Variable to control and manage items that player interacts with////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		int itemValue;									//this will randomly generate a number and then associate it with the relevant item
	UPROPERTY(EditAnywhere)
		FString itemText;								//this will hold the item text needed to be passed back to the player to inform them of the item
	UPROPERTY(EditAnywhere)
		TArray<FString> itemArray = { "Strength-1", "Strength-2", "Strength-3", "Vigor-1", "Vigor-2", "Vigor-3", "Masses-1", "Masses-2", "Masses-3" };
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 


	//Variables to hold all the editable variable value properties for the respective effects////////////////////////////////////////////////////////////////
	float projectileSpeed;
	float projectileMaxSpeed;
	float projectileLifeSpan;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	








	
	/*******************************************************************************PUBLIC FUNCTIONS***************************************************************************/
	
	//Functions to control spawning the interactable object////////////////////////////////////////////////////////////////////////////////////////////////
	FVector updateSpawnLocation();									//function that will calculate the spawn location of the 2nd object per level
	int spawnInteractable(FVector spawnLoc);						//function that will control the spawning of the interactable object
	void SetIsLevelComplete();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	//Functions to control player interaction with interactable////////////////////////////////////////////////////////////////////////////////////////////
	void playerTakesItem();											//responds to player input and gives player an item
	bool PlayerRerollItem();
	void getPlayerLocation(FVector playerLoc);						//function to get the current location of player to be used in distance measure
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions to control item generation, selection and display//////////////////////////////////////////////////////////////////////////////////////////
	void setItemValue();											//sets the value that will be used to determine the item passed to players
	void displayItemText();										//displays the item name / description to players
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions that call to relevant classes to apply item effects////////////////////////////////////////////////////////////////////////////////////////
	void callProjectileFunction(float x, float y, float z);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions to get variables from external classes/////////////////////////////////////////////////////////////////////////////////////////////////////
	void getRoomCount();
	void getPlacementModifier();


	UFUNCTION(BlueprintPure, Category = "Item Data")
		FString GetItemName();

	UFUNCTION(BlueprintPure, Category = "Item Data")
		float GetDistanceFromPlayer();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
