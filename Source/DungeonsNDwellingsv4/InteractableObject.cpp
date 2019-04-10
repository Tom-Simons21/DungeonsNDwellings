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
}


//Functions to control the core functionality of the interactable object////////////////////////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();

	setItemValue();
	getRoomCount();
	getPlacementModifier();
}

// Called every frame
void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isLevelComplete == true)
	{
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
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control the spawning of the interactable object/////////////////////////////////////////////////////////////////////////////////////////////////////
FVector AInteractableObject::updateSpawnLocation()
{
	secondSpawnPoint = startingSpawnPoint + (placementMod * roomCount);

	return (secondSpawnPoint);
}

int AInteractableObject::spawnInteractable(FVector spawnLoc)
{
		AInteractableObject* NewActor = GetWorld()->SpawnActor<AInteractableObject>(GetClass(), spawnLoc, FRotator::ZeroRotator);

		// Housekeeping so that we dont spawn new actors forever  
		NewActor->Iteration = Iteration - 1;
		Iteration = 0; // stop ourselves spawning any more 

		return Iteration;
}

void AInteractableObject::SetIsLevelComplete()
{
	isLevelComplete = true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control item generation, selection, display, management////////////////////////////////////////////////////////////////////////////////////////////
//function to generate a random number, each number will correspond to an item the player can collect.
void AInteractableObject::setItemValue()
{
	itemValue = FMath::RandRange(1, 3); //set's the value randomly between the range
}

/*void AInteractableObject::displayItemText()
{

}*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control the players interactions with the interactable object/////////////////////////////////////////////////////////////////////////////////////////
//function to get the current location of the player
void AInteractableObject::getPlayerLocation(FVector playerPos)
{
	FVector playerLoc = playerPos;
	FVector interactableLoc = GetActorLocation();

	distanceFromPlayer = FVector::Dist(playerLoc, interactableLoc); //checking if distance is < 120
}

//function to modify the players projectiles so that they will have a different effect after an item is taken
void AInteractableObject::playerTakesItem()
{
	if (distanceFromPlayer < 120)
	{
		//this should be replaced by a switch case sequence in this future, this is temporary for ease + testing
		if (itemValue == 1)
		{
			projectileSpeed = 500;
			projectileMaxSpeed = 500;
			projectileLifeSpan = 4;
			callProjectileFunction(projectileSpeed, projectileMaxSpeed, projectileLifeSpan);

			Destroy();
		}
		else if (itemValue == 2)
		{
			projectileSpeed = 1000;
			projectileMaxSpeed = 1000;
			projectileLifeSpan = 8;
			callProjectileFunction(projectileSpeed, projectileMaxSpeed, projectileLifeSpan);

			Destroy();
		}
		else if (itemValue == 3)
		{
			projectileSpeed = 1500;
			projectileMaxSpeed = 1500;
			projectileLifeSpan = 1.5;
			callProjectileFunction(projectileSpeed, projectileMaxSpeed, projectileLifeSpan);

			Destroy();
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to call to relevant classes to modify variables based on player item selection///////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








//Functions to GET and pass variables to external classes, all too be called in BeginPlay()////////////////////////////////////////////////////////////////////////
void AInteractableObject::getRoomCount()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->getRoomCount();
	}
}

void AInteractableObject::getPlacementModifier()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		placementMod = ActorItr->getRoomPlacementModifier();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////