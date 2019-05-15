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
	doorDefaults = { FVector(0, 400, 0), FVector(400, 800, 0), FVector(800, 400, 0), FVector(400, 0, 0) }; //default centre points for doors
}

//Functions to initialise and set up basic functionality on play///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADoorPathingManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Class Loaded")); //debug message
	arrayOfDoors.Empty();							//empty all arrays on level start
	doorStartPoints.Empty();						// " "
	doorMappings.Empty();							// " "

	GetRoomPlacementModifier();						//get distance between rooms
	GetRoomCount();									//get number of rooms
	CreateArrayOfDoors();							//call function to get an array of all existing doors
	CreateStartingPointArray();						//create array that lists all doors in literal order - i.e.

	/*****************************************************************************************************************************************************************/
	//
	//	------X------		------X------	
	//  |			|		|			|		Here you can see how the doors are numbered by default
	//  |			|		|			|		doors are counter by how many doors in location "X" going up
	//  |	room	|		|	room	|		so door 1 is first door in room 1, door 2 is first door in room 2,
	//  X	one		X		X	two		X		door 3 is first door in room 3.
	//  |			|		|			|		For this system this needed rearranging 
	//  |			|		|			|		for this it was needed to count doors in each room first
	//  |			|		|			|		so door 1 should be first door in room 1, but door 2 should be second door in
	//  ------1------		------2------		room 1, door 3 should be third door in room 1, door 4 is fourth door in room 1,
	//											door 5 would then be first door in room 2, assuming room 1 had four doors, if 
	//											it didnt then it would stop counting room 1 doors when it reached room 1 door num
	//
	/*****************************************************************************************************************************************************************/
	CreateExitPointArray();						//use entry point array to produce an array that plots how a player will be moved when they pass through a door
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Door mapping functions - this function creates an array of all doors in order of creation////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADoorPathingManager::CreateStartingPointArray()					//array listing doors in proper order explained above
{
	int doorCount;														//number of doors spawned
	FVector zLocation;													//where is the door, X/Y will be 1 of 4 postions Z = which room that door is in
	FVector doorLocation;												//specific location of door 1 of the 4 X/Y points + Z

	for (int i = 1; i < arrayOfDoors.Num(); i++)						//loop through once per room, array of doors holds number of doors in each room
	{
		doorCount = arrayOfDoors[i];									//check how many doors in room i
		zLocation = roomPlacementModifier * (i - 1);					//get the z location for room

		if (doorCount == 1)												//doors are allocated from the bottom anti-clockwise
		{
			doorLocation = doorDefaults[0] + zLocation;					//one door places player at the 0 index location
			doorStartPoints.Add(doorLocation);							//add to array
		}
		else if (doorCount == 2)
		{
			doorLocation = doorDefaults[0] + zLocation;
			doorStartPoints.Add(doorLocation);
			doorLocation = doorDefaults[1] + zLocation;					//2 doors will use 0 and 1 index locaitions
			doorStartPoints.Add(doorLocation);
		}
		else if (doorCount == 3)
		{
			doorLocation = doorDefaults[0] + zLocation;
			doorStartPoints.Add(doorLocation);
			doorLocation = doorDefaults[1] + zLocation;
			doorStartPoints.Add(doorLocation);
			doorLocation = doorDefaults[2] + zLocation;					//3 will use 0 - 2
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
			doorLocation = doorDefaults[3] + zLocation;					//4 uses all 4 index values
			doorStartPoints.Add(doorLocation);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function builds the exit points array////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This array plots a path from room 1 to end///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Then this room plots and pairs all remaining doors together - ensures each door always leads to a different room/////////////////////////////////////////////////////////////////////////////////
void ADoorPathingManager::CreateExitPointArray()			//pair doors together so a player passing through a door will move them to a predetermined position
{																	//section 1 plots a path from room 1 to the last room, guarantees level will be completable 
	int roomToAdd;													//selects a room from level - a door from this room will be paired
	int counter = roomCount - 1;									//always pass through less rooms than spawned on level
	int roomZCoord;													//room Z vector value
	bool isInArray;													//check we are selecting an unpaired value

	doorMappings.Add(doorStartPoints[0]);							//start with door 1
	do
	{
		roomToAdd = roomCount - counter;						//pick the next room - the rooms of choice will be predetermined, eventually though it will select a random pair of doors from these rooms completely randomising pathing
		roomZCoord = roomToAdd * roomPlacementModifier.Z;		//get the room Z
		for (int j = 0; j < doorStartPoints.Num(); j++)			//loop through all doors 
		{
			if (doorStartPoints[j].Z == roomZCoord)				//find door on the selected floor
			{
				roomsToAdd.Add(doorStartPoints[j]);				//add the door to a temporary array
				if ((j + 1) < doorStartPoints.Num())
				{
					roomsToAdd.Add(doorStartPoints[j + 1]);		//check if the next door in that room exist, currently these 2 doors are hard coded but these could be randomised in the future
				}
				break;											//once 2 doors are selected break
			}
		}
		counter -= 2;											//jump a room so if we added room 1 then went to room 2 we would then go to room 4
		if (counter == -1)										//counter must hit 0 at some point - this guarantees we reached the final room
		{
			counter = 0;
		}
	} while (counter > -1);										//go until counter hits -2 

	for (int k = 0; k < roomsToAdd.Num(); k++)					//once the path from start to end is in temp array, then we copy it to main exit array
	{
		doorMappings.Add(roomsToAdd[k]);						//copying over
	}
	for (int i = 0; i < doorStartPoints.Num(); i++)				//loop through array of doors
	{
		isInArray = false;
		for (int j = 0; j < doorMappings.Num(); j++)			//loop through the doors chosen for the key path
		{
			if (doorStartPoints[i] == doorMappings[j])			//check if the door being checked has been allocated on key path
			{
				isInArray = true;								//if yes set and break
				break;
			}
		}
		if (isInArray == false)									//if no 
		{
			remainingRooms.Add(doorStartPoints[i]);				//add to array that holds all not key path doors
		}
	}
	for (int g = 0; g < remainingRooms.Num(); g++)				//loop through all non key path doors
	{
		if (g < (remainingRooms.Num() / 2))						//add first half of remaining rooms to array 1
		{														//here we split remaining doors to ensure a door will not ever link to the same room it is from
			remainingRooms_One.Add(remainingRooms[g]);			//i.e. room 3 door 2 cannot link to room 3 door 4 etc.
		}
		else if (g >= (remainingRooms.Num() / 2))				//add the second half of the doors to array 2
		{
			remainingRooms_Two.Add(remainingRooms[g]);
		}
	}
	for (int s = 0; s < remainingRooms_One.Num(); s++)			//add all remaining doors to the mapping exit array
	{
		doorMappings.Add(remainingRooms_One[s]);				//door x from array 1 is added then
		doorMappings.Add(remainingRooms_Two[s]);				//door x from array 2 is added - therefore doors match against a door that is remaining doors/2 + door position
	}															//so if ther is 20 remaining doors - door 1 matches to door 11 which = 1 + 20/2
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions for getting key variables from other classes///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADoorPathingManager::CreateArrayOfDoors()				//gets the array of doors from generator
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		arrayOfDoors = ActorItr->GetArrayOfDoors();				//array of doors set
	}
}
void ADoorPathingManager::GetRoomPlacementModifier()		//get distance between rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->GetRoomPlacementModifier();
	}
}
void ADoorPathingManager::GetRoomCount()   //get number of rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->GetRoomCount();
	}
}
TArray<FVector> ADoorPathingManager::GetDoorMappingArray() //pass door mappings to player
{
	return (doorMappings);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



























//all version attempt at mapping///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
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
*/