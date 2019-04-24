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

	roomCount = 0;
}

//These functions control the core functionality of the Boss Manager////////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
void ABossManager::BeginPlay()
{
	Super::BeginPlay();

	//Initialise all variables from external classes asap////////////////////////////////////////////////////////////////////
	GetRoomCount();
	GetRoomPlacementModifier();
	GetPlayerZOffset();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//every level reset boss is killed to false//////////////////////////////////////////////////////////////////////////////
	isBossKilled = false;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Here goes the code to spawn the relevant boss//////////////////////////////////////////////////////////////////////////
	spawnLocation = spawnLocation + bossOffset;
	spawnLocation.Z += roomPlacementModifier * roomCount;

	spawnTrans = FTransform(spawnRotation, spawnLocation, spawnScale);

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		AProjectileBoss* projectileBoss = World->SpawnActor<AProjectileBoss>(AProjectileBoss::StaticClass(), spawnTrans);
	}

	for (TActorIterator<AProjectileBoss> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AProjectileBoss *Object = *ActorItr;
		currentBossDmg = ActorItr->GetBossProjectileDmg();
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

// Called every frame
void ABossManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control not specific boss behaviours///////////////////////////////////////////////////////////////////////////////////////////
void ABossManager::ActivateBoss(FVector playerLoc)
{
	float playerZ = playerLoc.Z;
	float floorVal = (playerZ - playerOffset) / roomPlacementModifier;

	if (floorVal == roomCount)
	{
		for (TActorIterator<AProjectileBoss> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AProjectileBoss *Object = *ActorItr;
			ActorItr->SetIsBossActive();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to implement level complete functionality////////////////////////////////////////////////////////////////////////////////////////
void ABossManager::SetIsBossKilled()
{
	isBossKilled = true;

	AddLevelCompleteElements();
}

void ABossManager::AddLevelCompleteElements()
{
	for (TActorIterator<AInteractableObjectManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AInteractableObjectManager *Object = *ActorItr;
		ActorItr->SpawnInteractableOnComplete();
	}

	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->SetLevelComplete();
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to GET or PASS required variables between seperate class objects/////////////////////////////////////////////////////////////////
void ABossManager::GetRoomCount()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->getRoomCount();
	}
}

void ABossManager::GetRoomPlacementModifier()
{
	FVector offset;

	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		offset = ActorItr->getRoomPlacementModifier();
	}

	roomPlacementModifier = offset.Z;
}

void ABossManager::GetPlayerZOffset()
{
	FVector offset;

	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		offset = ActorItr->GetPlayerZOffset();
	}

	playerOffset = offset.Z;
}

float ABossManager::GetZOffset()
{
	FVector current = GetActorLocation();
	float zOffset = (current.Z / roomCount) - roomPlacementModifier;

	return (zOffset);
}

float ABossManager::GetCurrentBossDmg()
{
	return currentBossDmg;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////