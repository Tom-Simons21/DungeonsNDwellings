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



private:
	/****************************************************************************PRIVATE VARIABLES**************************************************************************/

	//Variables to control player interaction with object///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		bool isInteractable;			//can the item be interacted with
	UPROPERTY(EditAnywhere)
		float distanceFromPlayer;										//this will track how far away the player is from the interactable
	UPROPERTY(EditAnywhere)
		bool isItemRerolled;				//has the item be rerolled
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Variable to control and manage items that player interacts with//////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		FString itemName;			//name of the item available
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Interactable object component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InteractableObject, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* ConeMesh;

	/*******************************************************************************PUBLIC FUNCTIONS***********************************************************************/
	
	//Functions to control player interaction with interactable////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void playerTakesItem();											//responds to player input and gives player an item
	bool PlayerRerollItem();				//player switches current item for a new one for a cost
	void GetPlayerDistance();										//function to get the current location of player to be used in distance measure
	void DisplayItemText();				//display the item name to player
	void RemoveItemText();				//remove the item name popup
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions to control item generation, selection and display//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SelectItem();												//sets the value that will be used to determine the item passed to players
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions to GET and SET variables from other classes////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float GetDistanceFromPlayer();			//how far away is the player
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
