// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemManager.h"
#include "InteractableObject.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "Engine.h"

// Sets default values
AItemManager::AItemManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	availableItemsCounter = 0;

	strItemCounter = 0;
	massItemCounter = 0;
	vigItemCounter = 0;

	isStrMaxed = false;
	isMassMaxed = false;
	isVigMaxed = false;

	currentAvailableItems.Empty();
}

// Called when the game starts or when spawned
void AItemManager::BeginPlay()
{
	Super::BeginPlay();
	
	regenRate = 0;
	activeDebuffs = { false, false, false };
}

// Called every frame
void AItemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Function to select items from the current item pool and allow player collections
void AItemManager::SelectItem(FString objectName)
{
	if (itemPool.Num() > 1)
	{
		itemValue = FMath::RandRange(1, itemPool.Num() - 1);

		if (objectName == "InteractableObject_0")
		{
			currentAvailableItems.Add(itemPool[itemValue]);

			itemPool.RemoveAt(itemValue);
		}
		else if (objectName == "InteractableObject_1")
		{
			currentAvailableItems.Add(itemPool[itemValue]);

			itemPool.RemoveAt(itemValue);
		}
	}
}

//Controlling functionality of player item interaction, setting items on reroll, setting items to player array on take///////////////////////////////////////////////////////
void AItemManager::RerollItem(FString objectName)
{
	//We use a lot of hard set variables in these functions because the current design is only to have 2 items per level
	if (itemPool.Num() > 1)
	{
		itemValue = FMath::RandRange(1, itemPool.Num() - 1);
	}
	if (objectName == "InteractableObject_0")
	{
		itemPool.Add(currentAvailableItems[0]);
		currentAvailableItems.Insert(itemPool[itemValue], 0);
		currentAvailableItems.RemoveAt(1);
		itemPool.RemoveAt(itemValue);
	}
	else if (objectName == "InteractableObject_1")
	{
		itemPool.Add(currentAvailableItems[1]);
		currentAvailableItems.Insert(itemPool[itemValue], 1);
		currentAvailableItems.RemoveAt(2);
		itemPool.RemoveAt(itemValue);
	}
}

void AItemManager::AddItemToPlayer(FString objectName)
{
	//We use a lot of hard set variables in these functions because the current design is only to have 2 items per level
	if (objectName == "InteractableObject_0")
	{
		playerItems.Add(currentAvailableItems[0]);
		currentAvailableItems.Insert(itemPool[0], 0);
		currentAvailableItems.RemoveAt(1);
	}
	else if (objectName == "InteractableObject_1")
	{
		playerItems.Add(currentAvailableItems[1]);
		currentAvailableItems.Insert(itemPool[0], 1);
		currentAvailableItems.RemoveAt(2);
	}
	TrackAffects();
	ApplyAffects();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control and apply affects to players based off of item selection//////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::TrackAffects()
{
	bool isNewClass;
	strItemCounter = 0;
	massItemCounter = 0;
	vigItemCounter = 0;

	for (int i = 0; i < playerItems.Num(); i++)
	{
		isNewClass = true;
		if (playerItems[i].Contains("Strength") == true)
		{
			strItemCounter++;
			if (uniqueClasses.Num() != 0)
			{
				for (int j = 0; j < uniqueClasses.Num(); j++)
				{
					if (uniqueClasses[j].Contains("Strength") == true)
					{
						isNewClass = false;
						break;
					}
				}
			}
			if (isNewClass == true)
			{
				uniqueClasses.AddUnique("Strength");
			}
		}
		else if (playerItems[i].Contains("Masses") == true)
		{
			massItemCounter++;
			if (uniqueClasses.Num() != 0)
			{
				for (int j = 0; j < uniqueClasses.Num(); j++)
				{
					if (uniqueClasses[j].Contains("Masses") == true)
					{
						isNewClass = false;
						break;
					}
				}
			}
			if (isNewClass == true)
			{
				uniqueClasses.AddUnique("Masses");
			}
		}
		else if (playerItems[i].Contains("Vigor") == true)
		{
			vigItemCounter++;
			if (uniqueClasses.Num() != 0)
			{
				for (int j = 0; j < uniqueClasses.Num(); j++)
				{
					if (uniqueClasses[j].Contains("Vigor") == true)
					{
						isNewClass = false;
						break;
					}
				}
			}
			if (isNewClass == true)
			{
				uniqueClasses.AddUnique("Vigor");
			}
		}
		else if (playerItems[i].Contains("Sacrifice") == true)
		{
			sacItemCounter++;
			if (uniqueClasses.Num() != 0)
			{
				for (int j = 0; j < uniqueClasses.Num(); j++)
				{
					if (uniqueClasses[j].Contains("Sacrifice") == true)
					{
						isNewClass = false;
						break;
					}
				}
			}
			if (isNewClass == true)
			{
				uniqueClasses.AddUnique("Sacrifice");
			}
		}
	}
}

void AItemManager::ApplyAffects()
{
	if (strItemCounter > 0)
	{
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetStrBuff();
		}
		StrAffects();
	}
	if (massItemCounter > 0)
	{
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetMassBuff();
		}
		MassesAffects();
	}
	if (vigItemCounter > 0)
	{
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetVigBuff();
		}
		VigAffects();
	}
	if (sacItemCounter > 0)
	{
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetSacBuff();
		}
		SacAffects();
	}

	if (uniqueClasses.Num() >= 3)
	{
		activeDebuffs[0] = true;

		if (uniqueClasses.Num() >= 6)
		{
			activeDebuffs[1] = true;

			if (uniqueClasses.Num() >= 9)
			{
				activeDebuffs[2] = true;
				DebuffAffects();
			}
			else
			{
				DebuffAffects();
			}
		}
		else
		{
			DebuffAffects();
		}
	}
}
//Functions for each family of affects, keep other two functions clean////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::StrAffects()
{
	isABuff = true;

	if (strItemCounter == 1)
	{
		damageMultiplier = 1.2;
	}
	else if (strItemCounter == 2)
	{
		damageMultiplier = 1.5;
	}
	else if (strItemCounter == 3)
	{
		damageMultiplier = 2;
		isStrMaxed = true;
	}
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->ModifyPlayerDamage(isABuff, damageMultiplier);
	}
}

void AItemManager::MassesAffects()
{
	isABuff = true;

	if (massItemCounter == 1)
	{
		spawnerModifier = 4;
	}
	else if (massItemCounter == 2)
	{
		spawnerModifier = 2;
	}
	else if (massItemCounter == 3)
	{
		spawnerModifier = 1;
		isMassMaxed = true;
	}
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->ModifyProjectileSpawnChance(isABuff, spawnerModifier);
	}
}

void AItemManager::VigAffects()
{
	isABuff = true;

	if (vigItemCounter == 1)
	{
		healthIncrease = 20;
	}
	else if (vigItemCounter == 2)
	{
		healthIncrease = 40;
	}
	else if (vigItemCounter == 3)
	{
		healthIncrease = 40;
		isHealthRegening = true;
		regenRate = 2;
		isVigMaxed = true;
	}
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->ModifyPlayerHealth(isABuff, healthIncrease, isHealthRegening, regenRate);
	}
}

void AItemManager::SacAffects()
{
	isABuff = true;

	if (sacItemCounter == 1)
	{
		healthToRecieve = 1;
		chanceToRecieve = 20; //100 / 20 = 5% chance
	}
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->ModifyPlayerKillBonuses(isABuff, healthToRecieve, chanceToRecieve);
	}
}


//Functions for debuffs and negative affects/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::DebuffAffects()
{
	isABuff = false;
	if (activeDebuffs[0] == true)
	{
		damageMultiplier = 0.75;
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->ModifyPlayerDamage(isABuff, damageMultiplier);
		}
	}
	if (activeDebuffs[1] == true)
	{
		healthIncrease = 0.5; //this will apply a half health effect
		isHealthRegening = false;
		regenRate = 0;
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->ModifyPlayerHealth(isABuff, healthIncrease, isHealthRegening, regenRate);
		}
	}
	if (activeDebuffs[2] == true)
	{
		healthIncrease = 1; //this will apply a half health effect
		isHealthRegening = true;
		regenRate = 0;
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->ModifyPlayerHealth(isABuff, healthIncrease, isHealthRegening, regenRate);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Public GET and SET functions///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TArray<FString> AItemManager::GetCurrentItems()
{
	return (currentAvailableItems);
}

bool AItemManager::IsStrBuffMaxed()
{
	return isStrMaxed;
}

bool AItemManager::IsMassBuffMaxed()
{
	return isMassMaxed;
}

bool AItemManager::IsVigBuffMaxed()
{
	return isVigMaxed;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




