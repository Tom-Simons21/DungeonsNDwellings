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




	int Iteration;									//integer variable to dictate how many times we "spawn" our interactable item

	float TimeToSpawn;								//spawns object whenever Iteration is true

	UPROPERTY(EditAnywhere)
	float distanceFromPlayer;						//this will track how far away the player is from the interactable

	UPROPERTY(EditAnywhere)
	int itemValue;									//this will randomly generate a number and then associate it with the relevant item

	bool isInteractable;							//this will determine whether an interactable object can be used (will be false after the user has used it)

	FString itemText;								//this will hold the item text needed to be passed back to the player to inform them of the item

	UPROPERTY(EditAnywhere)
	TArray<int32> itemArray;


	FVector updateSpawnLocation(int roomCount);		//function that will calculate the spawn location of the 2nd object per level
		
	int spawnInteractable(FVector spawnLoc);		//function that will control the spawning of the interactable object

	void updateIsInteractable();					//function to update the isInteractable variable, will interact with player object

	void getPlayerLocation(FVector playerLoc);		//function to get the current location of player to be used in distance measure

	float getDistanceFromPlayer(FVector playerPos);	//function to get the distance between player and object

	void setItemValue();							//sets the value that will be used to determine the item passed to players

	void playerTakesItem();							//responds to player input and gives player an item

	void callProjectileFunction(float x, float y, float z);

	void displayItemText();							//displays the item name / description to players
};
