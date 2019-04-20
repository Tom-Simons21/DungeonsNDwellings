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
#include "InteractableObjectManager.h"

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
}


//Functions to control the core functionality of the interactable object////////////////////////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();

	this->isInteractable = true;

	SelectItem();
}

// Called every frame
void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control item generation, selection, display, management////////////////////////////////////////////////////////////////////////////////////////////
//function to generate a random number, each number will correspond to an item the player can collect.
void AInteractableObject::SelectItem()
{
	FString name = this->GetName();

	for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AItemManager *Object = *ActorItr;
		ActorItr->SelectItem(name);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control the players interactions with the interactable object/////////////////////////////////////////////////////////////////////////////////////////
//function to get the current location of the player
void AInteractableObject::getPlayerLocation(FVector playerPos)
{
	bool isCloseEnough;
	FVector playerLoc = playerPos;
	FVector interactableLoc = GetActorLocation();

	distanceFromPlayer = FVector::Dist(playerLoc, interactableLoc); //checking if distance is < 120

	for (TActorIterator<AInteractableObjectManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AInteractableObjectManager *Object = *ActorItr;
		isCloseEnough = ActorItr->CheckDistanceFromPlayer();
	}

	if (isCloseEnough == false)
	{
		RemoveItemText();
	}
}

void AInteractableObject::DisplayItemText()
{
	AMyPlayerController* const MyPlayer = Cast<AMyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (MyPlayer != NULL)
	{
		MyPlayer->DisplayTextPopup();
	}
}

void AInteractableObject::RemoveItemText()
{
	AMyPlayerController* const MyPlayer = Cast<AMyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (MyPlayer != NULL)
	{
		MyPlayer->RemoveTextPopup();
	}
}

//function to modify the players projectiles so that they will have a different effect after an item is taken
void AInteractableObject::playerTakesItem()
{
	if (isInteractable == true)
	{		
		for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AItemManager *Object = *ActorItr;
			ActorItr->AddItemToPlayer(this->GetName());
		}
		isInteractable = false;
		itemName = " ";
		this->SetActorHiddenInGame(true);
		this->SetActorEnableCollision(false);
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Item already used."));
		}
	}
}

bool AInteractableObject::PlayerRerollItem()
{
	isItemRerolled = false;

	if (isInteractable == true)
	{
		for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AItemManager *Object = *ActorItr;
			ActorItr->RerollItem(this->GetName());
		}

		isItemRerolled = true;
	}
	return (isItemRerolled);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//Functions to GET and pass variables to external classes, all too be called in BeginPlay()////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////