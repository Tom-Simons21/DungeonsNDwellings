// Fill out your copyright notice in the Description page of Project Settings.

#include "DoorPathingManager.h"
#include "TileGeneratorParent.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "Engine.h"
#include "Engine/World.h"

// Sets default values
ADoorPathingManager::ADoorPathingManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	doorDefaults = { FVector(0, 400, 0), FVector(400, 800, 0), FVector(800, 400, 0), FVector(400, 0, 0) };
}

// Called when the game starts or when spawned
void ADoorPathingManager::BeginPlay()
{
	Super::BeginPlay();

	arrayOfDoors.Empty();
	doorStartPoints.Empty();
	doorEndPoints.Empty();


	GetRoomPlacementModifier();
	GetRoomCount();
	CreateArrayOfDoors();
	CreateStartingPointArray();
	CreateExitPointArray();
}

// Called every frame
void ADoorPathingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Door mapping functions////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADoorPathingManager::CreateStartingPointArray()
{
	int doorCount;
	FVector zLocation;
	FVector doorLocation;

	for (int i = 1; i < arrayOfDoors.Num(); i++)
	{
		doorCount = arrayOfDoors[i];
		zLocation = roomPlacementModifier * (i - 1);

		if (doorCount == 1)
		{
			doorLocation = doorDefaults[0] + zLocation;
			doorStartPoints.Add(doorLocation);
		}
		else if (doorCount == 2)
		{
			doorLocation = doorDefaults[0] + zLocation;
			doorStartPoints.Add(doorLocation);
			doorLocation = doorDefaults[1] + zLocation;
			doorStartPoints.Add(doorLocation);
		}
		else if (doorCount == 3)
		{
			doorLocation = doorDefaults[0] + zLocation;
			doorStartPoints.Add(doorLocation);
			doorLocation = doorDefaults[1] + zLocation;
			doorStartPoints.Add(doorLocation);
			doorLocation = doorDefaults[2] + zLocation;
			doorStartPoints.Add(doorLocation);
		}
		else if (doorCount == 4)
		{
			doorLocation = doorDefaults[0] + zLocation;
			doorStartPoints.Add(doorLocation);
			doorLocation = doorDefaults[1] + zLocation;
			doorStartPoints.Add(doorLocation);
			doorLocation = doorDefaults[2] + zLocation;
			doorStartPoints.Add(doorLocation);
			doorLocation = doorDefaults[3] + zLocation;
			doorStartPoints.Add(doorLocation);
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Error: Improper Value")));
			}
		}
	}
}

void ADoorPathingManager::CreateExitPointArray()
{
	int entryToMap;
	int exitToMap;
	FVector entryPoint;
	FVector exitPoint;
	bool isValidExit;
	bool isWorking;

	isWorking = true;

	
	doorEndPoints.Empty();

	for (int i = 0; i < doorStartPoints.Num(); i++)
	{
		doorEndPoints.Add(FVector(1, 1, 1));
	}

	for (int i = 0; i < (doorStartPoints.Num() / 2); i++)
	{
		do
		{
			isValidExit = true;

			entryToMap = FMath::RandRange(0, (doorStartPoints.Num() - 1));
			entryPoint = doorStartPoints[entryToMap];
			exitToMap = FMath::RandRange(0, (doorStartPoints.Num() - 1));
			exitPoint = doorStartPoints[exitToMap];

			if (entryToMap == exitToMap)
			{
				isValidExit = false;
			}
			else if (entryPoint.Z == exitPoint.Z)
			{
				isValidExit = false;
			}
			else if (entryPoint.Z == 0 && exitPoint.Z == (roomPlacementModifier.Z * roomCount))
			{
				isValidExit = false;
			}

			for (int j = 0; j < doorEndPoints.Num(); j++)
			{
				if (entryPoint == doorEndPoints[j])
				{
					isValidExit = false;
					break;
				}
				else if (exitPoint == doorEndPoints[j])
				{
					isValidExit = false;
					break;
				}
			}
		} while (isValidExit == false);

		doorEndPoints[exitToMap] = entryPoint;
		doorEndPoints[entryToMap] = exitPoint;
	}

	for (int k = 0; k < doorEndPoints.Num(); k++)
	{
		if (doorEndPoints[k].Z != 1)
		{
			if ((k + 1) < doorEndPoints.Num())
			{
				if (doorEndPoints[k + 1].Z != 1)
				{
					if (doorEndPoints[k].Z == doorEndPoints[k + 1].Z)
					{
						isWorking = false;
					}
				}
			}
		}
	}

	if (isWorking == false)
	{
		//UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
		GetWorld()->Exec(GetWorld(), TEXT("RestartLevel"));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions for getting key variables from other classes////////////////////////////////////////////////////////////////////////////////////////
void ADoorPathingManager::CreateArrayOfDoors()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		arrayOfDoors = ActorItr->getArrayOfDoors();
	}
}

void ADoorPathingManager::GetRoomPlacementModifier()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->getRoomPlacementModifier();
	}
}

void ADoorPathingManager::GetRoomCount()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->getRoomCount();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////