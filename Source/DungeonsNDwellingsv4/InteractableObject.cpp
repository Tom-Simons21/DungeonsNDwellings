// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "TileGeneratorParent.h"

// Sets default values
AInteractableObject::AInteractableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Static reference to the mesh to use for the Interactable object
	static ConstructorHelpers::FObjectFinder<UStaticMesh> InteractableObjectAsset(TEXT("/Game/TwinStick/Meshes/Cone.Cone"));

	// Create mesh component for the projectile sphere
	ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeMesh0"));
	RootComponent = ConeMesh;
	ConeMesh->SetStaticMesh(InteractableObjectAsset.Object);
	ConeMesh->SetupAttachment(RootComponent);
	SetActorScale3D(FVector(1, 1, 1));

	Iteration = 1;

	//itemArray.Empty();
	//itemArray.Init(0, 3);
}
// Called when the game starts or when spawned
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();

	setItemValue();
}
// Called every frame
void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	if (Iteration > 0)
	{
		TimeToSpawn -= DeltaTime;
		if (TimeToSpawn < 0.f)
		{
			// Make a location for the new actor to spawn at
			FVector NewLocation = updateSpawnLocation();
			
			//call function to spawn actor, passes back the value for iteration.
			Iteration = spawnInteractable(NewLocation);
		}
	}
	*/
}





int AInteractableObject::spawnInteractable(FVector spawnLoc)
{
		AInteractableObject* NewActor = GetWorld()->SpawnActor<AInteractableObject>(GetClass(), spawnLoc, FRotator::ZeroRotator);

		// Housekeeping so that we dont spawn new actors forever  
		NewActor->Iteration = Iteration - 1;
		Iteration = 0; // stop ourselves spawning any more 

		return Iteration;
}
void AInteractableObject::updateIsInteractable()
{
	isInteractable = false;
}
//function to get the current location of the player
void AInteractableObject::getPlayerLocation(FVector playerPos)
{
	FVector playerLoc = playerPos;
	FVector interactableLoc = GetActorLocation();

	distanceFromPlayer = FVector::Dist(playerLoc, interactableLoc); //checking if distance is < 125
}
//function to generate a random number, each number will correspond to an item the player can collect.
void AInteractableObject::setItemValue()
{
	itemValue = FMath::RandRange(1, 3); //set's the value randomly between the range

	/*
	bool isNumberUnique = true;

	do
	{
		if (itemArray[0] == 1 && itemArray[1] == 2 && itemArray[2] == 3)
		{
			itemValue = 0;
		}
		else if (itemArray[(itemValue - 1)] == itemValue)
		{
			isNumberUnique = false;
			itemValue = FMath::RandRange(1, 3); //set's the value randomly between the range
		}
		else
		{
			itemArray.Insert(itemValue, (itemValue - 1));
		}
	} while (isNumberUnique == false);
	*/
}
//function to modify the players projectiles so that they will have a different effect after an item is taken
void AInteractableObject::playerTakesItem()
{
	//place all modifyable properties here
	float objSpeed;
	float objSpeedMax;
	float objLifeSpan;

	if (distanceFromPlayer < 125)
	{

		//this should be replaced by a switch case sequence in this future, this is temporary for ease + testing
		if (itemValue == 1)
		{
			objSpeed = 500;
			objSpeedMax = 500;
			objLifeSpan = 4;

			callProjectileFunction(objSpeed, objSpeedMax, objLifeSpan);
		}
		else if (itemValue == 2)
		{
			objSpeed = 1000;
			objSpeedMax = 1000;
			objLifeSpan = 8;

			callProjectileFunction(objSpeed, objSpeedMax, objLifeSpan);
		}
		else if (itemValue == 3)
		{
			objSpeed = 1500;
			objSpeedMax = 1500;
			objLifeSpan = 1.5;

			callProjectileFunction(objSpeed, objSpeedMax, objLifeSpan);
		}
		else
		{
			objSpeed = 100;
			objSpeedMax = 100;
			objLifeSpan = 3;

			callProjectileFunction(objSpeed, objSpeedMax, objLifeSpan);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("All Items Used"));
			}
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Too Far Away"));
		}
	}
}
//function that calls to the projectile modification function, saves repeating it for every item variant
void AInteractableObject::callProjectileFunction(float x, float y, float z)
{
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->updateProjectileValues(x, y, z);
	}
}

/*
void AInteractableObject::displayItemText()
{

}
*/


FVector AInteractableObject::updateSpawnLocation()
{
	FVector spawnLocation;
	placementMod = getPlacementModifier();
	roomCount = getRoomCount();

	spawnLocation = FVector(400.f, 400.f, 50.f) + (placementMod * roomCount);

	return (spawnLocation);
}




int AInteractableObject::getRoomCount()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->getRoomCount();
	}

	return (roomCount);
}

FVector AInteractableObject::getPlacementModifier()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		placementMod = ActorItr->getRoomPlacementModifier();
	}

	return (placementMod);
}
