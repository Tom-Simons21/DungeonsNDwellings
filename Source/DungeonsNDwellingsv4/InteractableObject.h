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

	//Variables to control player interaction with object////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		bool isInteractable;
	UPROPERTY(EditAnywhere)
		float distanceFromPlayer;						//this will track how far away the player is from the interactable
	UPROPERTY(EditAnywhere)
		bool isItemRerolled;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Variable to control and manage items that player interacts with////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		FString itemName;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 



	








	
	/*******************************************************************************PUBLIC FUNCTIONS***************************************************************************/
	
	//Functions to control player interaction with interactable////////////////////////////////////////////////////////////////////////////////////////////
	void playerTakesItem();											//responds to player input and gives player an item
	bool PlayerRerollItem();
	void getPlayerLocation(FVector playerLoc);						//function to get the current location of player to be used in distance measure
	void DisplayItemText();
	void RemoveItemText();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions to control item generation, selection and display//////////////////////////////////////////////////////////////////////////////////////////
	void SelectItem();											//sets the value that will be used to determine the item passed to players
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
