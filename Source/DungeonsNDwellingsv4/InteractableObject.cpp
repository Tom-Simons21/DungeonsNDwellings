// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "TileGeneratorParent.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "MyPlayerController.h"
#include "ItemManager.h"

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

	SelectItem();
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
void AInteractableObject::SelectItem()
{
	for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AItemManager *Object = *ActorItr;
		ActorItr->SelectItem();
	}

	SetItemName();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control the players interactions with the interactable object/////////////////////////////////////////////////////////////////////////////////////////
//function to get the current location of the player
void AInteractableObject::getPlayerLocation(FVector playerPos)
{
	FVector playerLoc = playerPos;
	FVector interactableLoc = GetActorLocation();

	distanceFromPlayer = FVector::Dist(playerLoc, interactableLoc); //checking if distance is < 120

	if (distanceFromPlayer < 120)
	{
		AMyPlayerController* const MyPlayer = Cast<AMyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
		if (MyPlayer != NULL)
		{
			MyPlayer->DisplayTextPopup();
		}
	}
	else
	{
		AMyPlayerController* const MyPlayer = Cast<AMyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
		if (MyPlayer != NULL)
		{
			MyPlayer->RemoveTextPopup();
		}
	}
}

//function to modify the players projectiles so that they will have a different effect after an item is taken
void AInteractableObject::playerTakesItem()
{
	if (distanceFromPlayer < 120)
	{
		Destroy();
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Too Far Away"));
		}
	}
}

bool AInteractableObject::PlayerRerollItem()
{
	isItemRerolled = false;

	if (distanceFromPlayer < 120)
	{
		for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AItemManager *Object = *ActorItr;
			ActorItr->RerollItem(this->GetName());
		}

		for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AItemManager *Object = *ActorItr;
			itemName = ActorItr->GetItemName();
		}

		isItemRerolled = true;
	}

	return (isItemRerolled);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





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

void AInteractableObject::SetItemName()
{
	for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AItemManager *Object = *ActorItr;
		itemName = ActorItr->GetItemName();
	}
}

FString AInteractableObject::GetItemName()
{
	return (itemName);
}

float AInteractableObject::GetDistanceFromPlayer()
{
	return (distanceFromPlayer);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////