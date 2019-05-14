// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObjectManager.h"
#include "InteractableObject.h"
#include "TileGeneratorParent.h"
#include "ItemManager.h"
#include "Engine.h"

// Sets default values
AInteractableObjectManager::AInteractableObjectManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	interactableObjectArray.Empty();		//at the start there are no interactables
	itemSpawnedCounter = 0;					//count how many are present 1 or 2
}

//Functions to control basic functionality/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AInteractableObjectManager::BeginPlay()
{
	Super::BeginPlay();

	GetRoomCount();						//number of rooms
	GetRoomPlacementModifier();			//distance between rooms
	
	for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AItemManager *Object = *ActorItr;
		ActorItr->GetLevelNumber();			//tell item manager to check the level
		ActorItr->GetItemStatsFromGI();		//tell item manager to load items from game instance
	}
	SpawnInteractableObject();				//spawn first interactable
}
void AInteractableObjectManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control interactable object spawning///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AInteractableObjectManager::SpawnInteractableObject()
{
	itemSpawnedCounter++;			//increment number of interactables
	FTransform transform;			//create a transform

	UWorld* const World = GetWorld();
	if (itemSpawnedCounter == 1)		//first item spawn conditions
	{
		transform = FTransform(objectRotator, objectSpawn, objectScale); //spawns at default point
		if (World != NULL)
		{
			AInteractableObject* NewActor = World->SpawnActor<AInteractableObject>(AInteractableObject::StaticClass(), transform); //spawn
			interactableObjectArray.Add(NewActor); //add to array
		}
	}
	else if (itemSpawnedCounter == 2) //for second item
	{ 
		objectSpawn += (roomCount * roomPlacementModifier);	//spawn in final room
		transform = FTransform(objectRotator, objectSpawn, objectScale);	//adjusted transform
		if (World != NULL)
		{
			AInteractableObject* NewActor = World->SpawnActor<AInteractableObject>(AInteractableObject::StaticClass(), transform); //spawn
			interactableObjectArray.Add(NewActor); //add to array
		}
	}
}
void AInteractableObjectManager::SpawnInteractableOnComplete()
{
	SpawnInteractableObject();	//spawn again
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to apply functionality to interactable object - functions are applied here to ensure they apply to the correct 1 if 2 are currently spawned///////////////////////////////
bool AInteractableObjectManager::CheckDistanceFromPlayer()
{
	bool isCloseEnough = false; //is player within 120 distance

	for (int i = 0; i < interactableObjectArray.Num(); i++)		//check all current interactables
	{
		distance = interactableObjectArray[i]->GetDistanceFromPlayer();		//check for each
		if (distance < minimumDistanceToInteract)	//if distance < 120
		{
			isCloseEnough = true;	
			interactableObjectArray[i]->DisplayItemText(); //display the item popup
			break;
		}
	}
	return (isCloseEnough);	
}
bool AInteractableObjectManager::Reroll()
{
	bool isRerolled = false; //did the player reroll

	for (int i = 0; i < interactableObjectArray.Num(); i++)  //check all interactables
	{
		distance = interactableObjectArray[i]->GetDistanceFromPlayer();	//player must be within distance
		if (distance < minimumDistanceToInteract)
		{
			isRerolled = interactableObjectArray[i]->PlayerRerollItem();	//checks the conditions on reroll function on object
			break; //gets the return i.e. player can press the button to reroll but the interactable must be usable and player must be in range to provide true return
		}
	}
	return (isRerolled);
}
void AInteractableObjectManager::Claim()		//player takes item
{
	for (int i = 0; i < interactableObjectArray.Num(); i++)	//check against all interactables
	{
		distance = interactableObjectArray[i]->GetDistanceFromPlayer();
		if (distance < minimumDistanceToInteract)
		{
			interactableObjectArray[i]->playerTakesItem();	//add item to player - rest of functionality occurs on specific actor
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to GET and pass variables to external classes, all too be called in BeginPlay()///////////////////////////////////////////////////////////////////////////////////////////
void AInteractableObjectManager::GetRoomCount()			//gets number of rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->GetRoomCount();
	}
}
void AInteractableObjectManager::GetRoomPlacementModifier()			//gets distance between rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->GetRoomPlacementModifier();
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
