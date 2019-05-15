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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> InteractableObjectAsset(TEXT("/Game/TwinStick/Meshes/Cone.Cone")); //select asset

	// Create mesh component for the projectile sphere
	ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeMesh0"));	//create mesh object
	RootComponent = ConeMesh;	//set root
	ConeMesh->SetStaticMesh(InteractableObjectAsset.Object);	//attach to asset
	//ConeMesh->SetupAttachment(RootComponent);
	SetActorScale3D(FVector(1, 1, 1));		//scale is default

	isInteractable = true;  //item is usable on spawn
}


//Functions to control the core functionality of the interactable object////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
	SelectItem();			//selects an item as soon as it is spawned
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
	FString name = GetName();	//selects its own name so item manager know which interactable to apply the item to

	for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AItemManager *Object = *ActorItr;
		ActorItr->SelectItem(name);		//calls to item manager for an item
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control the players interactions with the interactable object////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AInteractableObject::GetPlayerDistance() //function to get the current location of the player
{
	bool isCloseEnough = false;			//checks if player is close enough
	FVector playerLocation;				//players current position
	FVector objectLocation = GetActorLocation();	//position of this particular interactable

	//Old system but due to my obsessive fear of changing stuff this close to hand in this will be left even though I know it does nothing////////
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr) //get the players location
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		playerLocation = ActorItr->GetPlayersCurrentLocation();	//get distance from player
	}
	distanceFromPlayer = FVector::Dist(playerLocation, objectLocation); //how far is the player from an interactable item
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for (TActorIterator<AInteractableObjectManager> ActorItr(GetWorld()); ActorItr; ++ActorItr) //check if player is close enough, manager provides key functionality
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AInteractableObjectManager *Object = *ActorItr;
		isCloseEnough = ActorItr->CheckDistanceFromPlayer(); //is player close enough
	}
	if (isCloseEnough == false) //when player is too far away
	{
		RemoveItemText(); //remove text when player is out of range
	}
}
void AInteractableObject::DisplayItemText() //display item text widget
{
	if (GEngine)
	{
		AMyPlayerController* const MyPlayer = Cast<AMyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())); //cast to player controller
		if (MyPlayer != NULL) //if player is valid
		{
			MyPlayer->DisplayTextPopup();  //display text popup
		}
	}
}
void AInteractableObject::RemoveItemText() //remove item text widget
{
	if (GEngine)
	{
		AMyPlayerController* const MyPlayer = Cast<AMyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld())); //cast
		if (MyPlayer != NULL)
		{
			MyPlayer->RemoveTextPopup(); //remove text popup
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to interact with the interactable items, these allow the player to pick up or reroll the item provided//////////////////////////////////////////////////////////////////////////////
void AInteractableObject::playerTakesItem() 
{
	if (isInteractable == true) //if can be used
	{		
		for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AItemManager *Object = *ActorItr;
			ActorItr->AddItemToPlayer(GetName()); //add the item to the player
		}
		isInteractable = false;	//once taken interactable is used up
		itemName = " ";			//change item name to blank
		SetActorHiddenInGame(true);	//hide interactable
		SetActorEnableCollision(false);	//remove collision
	}
	else
	{
		//if (GEngine) //currently commenting out debug for use in demo etc
		//{
		//    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Item already used."));
		//}
	}
}
bool AInteractableObject::PlayerRerollItem()
{
	isItemRerolled = false; 

	if (isInteractable == true) //can be interacted with
	{
		for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AItemManager *Object = *ActorItr;
			ActorItr->RerollItem(GetName());	//function on item manager to switch available items
		}
		isItemRerolled = true;
	}
	return (isItemRerolled);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to GET and pass variables to external classes, all too be called in BeginPlay()/////////////////////////////////////////////////////////////////////////////////////////////////////
float AInteractableObject::GetDistanceFromPlayer()
{
	return (distanceFromPlayer); //gets the distance from the player
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////