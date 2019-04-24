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

}

// Called when the game starts or when spawned
void AInteractableObjectManager::BeginPlay()
{
	Super::BeginPlay();

	GetRoomCount();
	GetRoomPlacementModifier();

	interactableObjectArray.Empty();
	itemSpawnedCounter = 0;

	for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AItemManager *Object = *ActorItr;
		ActorItr->GetLevelNumber();
		ActorItr->GetItemStatsFromGI();
	}

	SpawnInteractableObject();
}

// Called every frame
void AInteractableObjectManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableObjectManager::SpawnInteractableObject()
{
	itemSpawnedCounter++;
	FTransform transform;

	UWorld* const World = GetWorld();
	if (itemSpawnedCounter == 1)
	{
		transform = FTransform(objectRotator, objectSpawn, objectScale);

		if (World != NULL)
		{
			AInteractableObject* NewActor = World->SpawnActor<AInteractableObject>(AInteractableObject::StaticClass(), transform);
			
			interactableObjectArray.Add(NewActor);
		}
	}
	else if (itemSpawnedCounter == 2)
	{
		objectSpawn += (roomCount * roomPlacementModifier);
		transform = FTransform(objectRotator, objectSpawn, objectScale);
		
		if (World != NULL)
		{
			AInteractableObject* NewActor = World->SpawnActor<AInteractableObject>(AInteractableObject::StaticClass(), transform);

			interactableObjectArray.Add(NewActor);
		}
	}
}

void AInteractableObjectManager::SpawnInteractableOnComplete()
{
	SpawnInteractableObject();
}

bool AInteractableObjectManager::CheckDistanceFromPlayer()
{
	bool isCloseEnough = false;

	for (int i = 0; i < interactableObjectArray.Num(); i++)
	{
		distance = interactableObjectArray[i]->distanceFromPlayer;

		if (distance < minimumDistanceToInteract)
		{
			isCloseEnough = true;
			interactableObjectArray[i]->DisplayItemText();
			break;
		}
	}
	return (isCloseEnough);
}

bool AInteractableObjectManager::Reroll()
{
	bool isRerolled = false;

	for (int i = 0; i < interactableObjectArray.Num(); i++)
	{
		distance = interactableObjectArray[i]->distanceFromPlayer;

		if (distance < minimumDistanceToInteract)
		{
			isRerolled = interactableObjectArray[i]->PlayerRerollItem();
			break;
		}
	}
	return (isRerolled);
}

void AInteractableObjectManager::Claim()
{
	for (int i = 0; i < interactableObjectArray.Num(); i++)
	{
		distance = interactableObjectArray[i]->distanceFromPlayer;

		if (distance < minimumDistanceToInteract)
		{
			interactableObjectArray[i]->playerTakesItem();
			break;
		}
	}
}

//Functions to GET and pass variables to external classes, all too be called in BeginPlay()////////////////////////////////////////////////////////////////////////
void AInteractableObjectManager::GetRoomCount()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->getRoomCount();
	}
}

void AInteractableObjectManager::GetRoomPlacementModifier()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->getRoomPlacementModifier();
	}
}
