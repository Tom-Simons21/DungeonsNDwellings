// Fill out your copyright notice in the Description page of Project Settings.

#include "BossManager.h"
#include "ProjectileBoss.h"
#include "Engine.h"
#include "Engine/World.h"
#include "TileGeneratorParent.h"

// Sets default values
ABossManager::ABossManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	roomCount = 0;
}

// Called when the game starts or when spawned
void ABossManager::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const World = GetWorld();

	if (World != NULL)
	{
		AProjectileBoss* projectileBoss = World->SpawnActor<AProjectileBoss>(AProjectileBoss::StaticClass(), spawnTrans);
	}
}

// Called every frame
void ABossManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossManager::ActivateBoss(FVector playerLoc)
{
	float playerZ = playerLoc.Z;
	float floorVal = (playerZ - playerOffset) / roomOffset;

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

void ABossManager::GetRoomCount()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->getRoomCount();
	}
}

