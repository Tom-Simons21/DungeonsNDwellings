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

void AItemManager::SelectItem()
{
	if (itemPool.Num() > 1)
	{
		itemValue = FMath::RandRange(1, itemPool.Num() - 1);

		currentAvailableItems.Add(itemPool[itemValue]);
		availableItemsCounter += 1;
		itemName = currentAvailableItems[availableItemsCounter - 1];

		itemPool.RemoveAt(itemValue);
	}
}

void AItemManager::RerollItem(FString objectName)
{
	if (itemPool.Num() > 1)
	{
		itemValue = FMath::RandRange(1, itemPool.Num() - 1);
	}

	if (objectName == "InteractableObject0")
	{
		itemPool.Add(currentAvailableItems[0]);
		currentAvailableItems.Insert(itemPool[itemValue], 0);
		itemName = currentAvailableItems[0];
		currentAvailableItems.RemoveAt(1);
		itemPool.RemoveAt(itemValue);
	}
	else if (objectName == "InteractableObject_0")
	{
		itemPool.Add(currentAvailableItems[1]);
		currentAvailableItems.Insert(itemPool[itemValue], 1);
		itemName = currentAvailableItems[1];
		currentAvailableItems.RemoveAt(2);
		itemPool.RemoveAt(itemValue);
	}
}



//Public GET and SET functions////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FString AItemManager::GetItemName()
{
	return (itemName);
}



