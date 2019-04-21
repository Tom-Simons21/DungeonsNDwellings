// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemManager.h"
#include "InteractableObject.h"
#include "Engine.h"

// Sets default values
AItemManager::AItemManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	availableItemsCounter = 0;

	currentAvailableItems.Empty();
}

// Called when the game starts or when spawned
void AItemManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Function to select items from the current item pool and allow player collections
void AItemManager::SelectItem(FString objectName)
{
	if (itemPool.Num() > 1)
	{
		itemValue = FMath::RandRange(1, itemPool.Num() - 1);

		if (objectName == "InteractableObject_0")
		{
			currentAvailableItems.Add(itemPool[itemValue]);

			itemPool.RemoveAt(itemValue);
		}
		else if (objectName == "InteractableObject_1")
		{
			currentAvailableItems.Add(itemPool[itemValue]);

			itemPool.RemoveAt(itemValue);
		}
	}
}


//Controlling functionality of player item interaction, setting items on reroll, setting items to player array on take///////////////////////////////////////////////////////
void AItemManager::RerollItem(FString objectName)
{
	//We use a lot of hard set variables in these functions because the current design is only to have 2 items per level
	if (itemPool.Num() > 1)
	{
		itemValue = FMath::RandRange(1, itemPool.Num() - 1);
	}

	if (objectName == "InteractableObject_0")
	{
		itemPool.Add(currentAvailableItems[0]);
		currentAvailableItems.Insert(itemPool[itemValue], 0);
		currentAvailableItems.RemoveAt(1);
		itemPool.RemoveAt(itemValue);
	}
	else if (objectName == "InteractableObject_1")
	{
		itemPool.Add(currentAvailableItems[1]);
		currentAvailableItems.Insert(itemPool[itemValue], 1);
		currentAvailableItems.RemoveAt(2);
		itemPool.RemoveAt(itemValue);
	}
}

void AItemManager::AddItemToPlayer(FString objectName)
{
	//We use a lot of hard set variables in these functions because the current design is only to have 2 items per level
	if (objectName == "InteractableObject_0")
	{
		playerItems.Add(currentAvailableItems[0]);
		currentAvailableItems.Insert(itemPool[0], 0);
		currentAvailableItems.RemoveAt(1);
	}
	else if (objectName == "InteractableObject_1")
	{
		playerItems.Add(currentAvailableItems[1]);
		currentAvailableItems.Insert(itemPool[0], 1);
		currentAvailableItems.RemoveAt(2);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//Public GET and SET functions///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<FString> AItemManager::GetCurrentItems()
{
	return (currentAvailableItems);
}



