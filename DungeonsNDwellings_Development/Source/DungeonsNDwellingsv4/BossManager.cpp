// Fill out your copyright notice in the Description page of Project Settings.

#include "BossManager.h"
#include "ProjectileBoss.h"
#include "Engine.h"
#include "Engine/World.h"
#include "TileGeneratorParent.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "InteractableObjectManager.h"

// Sets default values
ABossManager::ABossManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	roomCount = 0;										//default room count
}

//These functions control the core functionality of the Boss Manager////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ABossManager::BeginPlay()
{
	Super::BeginPlay();
	//Initialise all variables from external classes asap////////////////////////////////////////////////////////////////////
	GetRoomCount();								//gets how many rooms there are
	GetRoomPlacementModifier();					//gets distance between rooms
	GetPlayerZOffset();							//gets player distance off ground
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//every level reset boss is killed to false///////////////////////////////////////////////////////////////////////////////
	isBossKilled = false;						//on begin level boss will not be dead
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Here goes the code to spawn the relevant boss///////////////////////////////////////////////////////////////////////////
	spawnLocation = spawnLocation + bossOffset;											//boss spawn is currently hard set due to only 1 boss
	spawnLocation.Z += roomPlacementModifier * roomCount;								//spawn in final room
	spawnTrans = FTransform(spawnRotation, spawnLocation, spawnScale);					//set transform

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		AProjectileBoss* projectileBoss = World->SpawnActor<AProjectileBoss>(AProjectileBoss::StaticClass(), spawnTrans); //spawn boss in centre of final room
	}
	for (TActorIterator<AProjectileBoss> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AProjectileBoss *Object = *ActorItr;
		currentBossDmg = ActorItr->GetBossProjectileDmg();												//get the damage this boss deals this is in place for when more bosses exist
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control not specific boss behaviours/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ABossManager::ActivateBoss(FVector playerLoc)																	//turn boss on
{
	float playerZ = playerLoc.Z;																					//get player Z location
	float floorVal = (playerZ - playerOffset) / roomPlacementModifier;												//check which floor player is on

	if (floorVal == roomCount)																						//if player is on same floor as boss - boss turns on
	{
		for (TActorIterator<AProjectileBoss> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AProjectileBoss *Object = *ActorItr;
			ActorItr->SetIsBossActive();																			//turn on
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to implement level complete functionality///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ABossManager::SetIsBossKilled()																			//if boss is killed
{
	isBossKilled = true;																						//boss died 
	AddLevelCompleteElements();																					//add functions only available after level "completion"
}
void ABossManager::AddLevelCompleteElements()
{
	for (TActorIterator<AInteractableObjectManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AInteractableObjectManager *Object = *ActorItr;
		ActorItr->SpawnInteractableOnComplete();																//second interactable spawns when boss dies
	}
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->SetLevelComplete();																			//tells player object level is complete - activates further interactions
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to GET or PASS required variables between seperate class objects////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ABossManager::GetRoomCount()																				//checks number of rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->GetRoomCount();																	//gets from generator parent
	}
}
void ABossManager::GetRoomPlacementModifier()																	//gets distance vector
{
	FVector offset;																								//holds vector distance
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		offset = ActorItr->GetRoomPlacementModifier();															//set vector distance	
	}
	roomPlacementModifier = offset.Z;																			//convert to float
}
void ABossManager::GetPlayerZOffset()
{
	FVector offset;																								//hold offset distance
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		offset = ActorItr->GetPlayerZOffset();
	}
	playerOffset = offset.Z;																					//convert to float
}
float ABossManager::GetZOffset()																				//pretty sure this is never used
{																												//leaving it in place though as of right now
	FVector current = GetActorLocation();																		//do not want to risk removing a script at this point
	float zOffset = (current.Z / roomCount) - roomPlacementModifier;											//literally does nothing so no risk
	return (zOffset);																							
}
float ABossManager::GetCurrentBossDmg()																			//gets current boss damage
{
	return currentBossDmg;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////