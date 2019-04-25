// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
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

	isInteractable = true;
}


//Functions to control the core functionality of the interactable object////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
	SelectItem();
}

// Called every frame
void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control item generation, selection, display, management//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AInteractableObject::SelectItem() //function to generate a random number, each number will correspond to an item the player can collect
{
	FString name = this->GetName();

	for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AItemManager *Object = *ActorItr;
		ActorItr->SelectItem(name);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control the players interactions with the interactable object////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AInteractableObject::GetPlayerDistance() //function to get the current location of the player
{
	bool isCloseEnough = false;
	FVector playerLocation;
	FVector objectLocation = GetActorLocation();

	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr) //get the players location
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		playerLocation = ActorItr->GetPlayersCurrentLocation();
	}

	distanceFromPlayer = FVector::Dist(playerLocation, objectLocation); //how far is the player from an interactable item

	for (TActorIterator<AInteractableObjectManager> ActorItr(GetWorld()); ActorItr; ++ActorItr) //check if player is close enough, manager provides key functionality
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AInteractableObjectManager *Object = *ActorItr;
		isCloseEnough = ActorItr->CheckDistanceFromPlayer();
	}
	if (isCloseEnough == false)
	{
		RemoveItemText(); //remove text when player is out of range
	}
}
void AInteractableObject::DisplayItemText() //display item text widget
{
	AMyPlayerController* const MyPlayer = Cast<AMyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (MyPlayer != NULL)
	{
		MyPlayer->DisplayTextPopup();
	}
}
void AInteractableObject::RemoveItemText() //remove item text widget
{
	AMyPlayerController* const MyPlayer = Cast<AMyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (MyPlayer != NULL)
	{
		MyPlayer->RemoveTextPopup();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to interact with the interactable items, these allow the player to pick up or reroll the item provided//////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to GET and pass variables to external classes, all too be called in BeginPlay()/////////////////////////////////////////////////////////////////////////////////////////////////////
float AInteractableObject::GetDistanceFromPlayer()
{
	return (distanceFromPlayer);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////