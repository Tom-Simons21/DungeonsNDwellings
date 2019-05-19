#include "ItemManager.h"
#include "InteractableObject.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "Engine.h"
#include "DungeonsNDwellingsInstance.h"

// Sets default values
AItemManager::AItemManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//inialise item counters
	availableItemsCounter = 0;		//number of items currently accessible to player
	strItemCounter = 0;				//number of strength items collected
	massItemCounter = 0;			//multishot item counter
	vigItemCounter = 0;				//health item counter
	sacItemCounter = 0;				//sacrafice item counter
	rateItemCounter = 0;			//fire rate item counter
	growthItemCounter = 0;			//projectile growth item counter
	slowItemCounter = 0;			//enemy slow down item counter
	hyperItemCounter = 0;			//hyper mode counter
	moneyShotItemCounter = 0;		//money drop increase counter

	//on start no buffs should be maxed
	isStrMaxed = false;				//has all of an item of the specified type
	isMassMaxed = false;
	isVigMaxed = false;

	//initialise all variables prior to ensure all are initialised
	damageMultiplier = 1;			//setting all variables to defaults
	spawnerModifier = 1;
	healthIncrease = 0;
	isHealthRegening = false;
	regenRate = 0;
	healthToRecieve = 0;
	chanceToRecieve = 0;
	fireRate = 0;
	initialSpeed = 0;
	maxSpeed = 0;
	lifeSpan = 0;
	isGrowing = false;
	scale = FVector(1, 1, 1);
	speedReductionPercent = 1;
	percentDamageIncrease = 1;
	dropChanceIncrease = 0;

	//clear + initialise all arrays
	activeDebuffs = { false, false, false };		//at start all buffs should be inactive
	appliedDebuffs = { false, false , false };		//all debuffs should start inactive
	currentAvailableItems.Empty();					//no items available on start
	playerItems.Empty();							//player has no items
	uniqueClasses.Empty();							//therefore no unique classes
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Basic functionality functions////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AItemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function to select items from the current item pool and allow player collections
void AItemManager::SelectItem(FString objectName)
{
	if (itemPool.Num() > 1) //if item pool has more than the blank item in it
	{
		itemValue = FMath::RandRange(1, itemPool.Num() - 1);	//get a value other than the blank value - blank is at index 0

		if (objectName == "InteractableObject_0")		//check which interactable object to attach to
		{
			currentAvailableItems.Add(itemPool[itemValue]);	//add the item from item pool at the generated value
			itemPool.RemoveAt(itemValue);					//if item has been added to current then remove from pool
		}
		else if (objectName == "InteractableObject_1")
		{
			currentAvailableItems.Add(itemPool[itemValue]);
			itemPool.RemoveAt(itemValue);
		}
	}
}

//Controlling functionality of player item interaction, setting items on reroll, setting items to player array on take/////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::RerollItem(FString objectName)		
{
	//We use a lot of hard set variables in these functions because the current design is only to have 2 items per level
	if (itemPool.Num() > 1)		//check if item pool has more item in it
	{
		itemValue = FMath::RandRange(1, itemPool.Num() - 1); //if it does select one
	}
	if (objectName == "InteractableObject_0")	//check which object we are passing too
	{
		itemPool.Add(currentAvailableItems[0]);		//pass the current item back to the item pool
		currentAvailableItems.Insert(itemPool[itemValue], 0); //take the new item from pool and insert at the relative location
		currentAvailableItems.RemoveAt(1);						//remove the boosted value at relative location + 1
		itemPool.RemoveAt(itemValue);							//remove new value from item pool
	}
	else if (objectName == "InteractableObject_1")			
	{
		itemPool.Add(currentAvailableItems[1]);		//same system but for second interactable, for this we increment all hard set values by 1
		currentAvailableItems.Insert(itemPool[itemValue], 1);
		currentAvailableItems.RemoveAt(2);
		itemPool.RemoveAt(itemValue);
	}
}

void AItemManager::AddItemToPlayer(FString objectName)  //adding items to player inventory or collected pool etc
{
	//We use a lot of hard set variables in these functions because the current design is only to have 2 items per level
	if (objectName == "InteractableObject_0")			//check which object we are referencing
	{
		playerItems.Add(currentAvailableItems[0]);		//item the item from this object to player
		currentAvailableItems.Insert(itemPool[0], 0);	//remove item from available pool, and replace with a blank item
		currentAvailableItems.RemoveAt(1);
	}
	else if (objectName == "InteractableObject_1")
	{
		playerItems.Add(currentAvailableItems[1]);		//same as above but increment 1
		currentAvailableItems.Insert(itemPool[0], 1);
		currentAvailableItems.RemoveAt(2);
	}
	TrackAffects();				//track the affects being applied to player
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control and apply affects to players based off of item selection////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::TrackAffects()
{
	int valueToCheck = (playerItems.Num() - 1); //check the last item added
	//this general functionaliy is repeated to some extent with every item/item type the change is the item checked and the function names as they relate to that item
	if (playerItems[valueToCheck].Contains("Strength") == true)		//check what type of item it is
	{
		strItemCounter++;					//increment the respective counter
		UpdateUniqueArray("Strength");
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetStrBuff();							//activate the respective buff on the player
		}
		StrAffects();	//call function to apply the affect
	}
	else if (playerItems[valueToCheck].Contains("Masses") == true)
	{
		massItemCounter++;
		UpdateUniqueArray("Masses");
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetMassBuff();
		}
		MassesAffects();
	}
	else if (playerItems[valueToCheck].Contains("Vigor") == true)
	{
		vigItemCounter++;
		UpdateUniqueArray("Vigor");
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetVigBuff();
		}
		VigAffects();
	}
	else if (playerItems[valueToCheck].Contains("Sacrifice") == true)
	{
		sacItemCounter++;
		UpdateUniqueArray("Sacrifice");
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetSacBuff();
		}
		SacAffects();
	}
	else if (playerItems[valueToCheck].Contains("Fire Rate") == true)
	{
		rateItemCounter++;
		UpdateUniqueArray("Fire Rate");
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetRateBuff();
		}
		RateAffects();
	}
	else if (playerItems[valueToCheck].Contains("Growth") == true)
	{
		growthItemCounter++;
		UpdateUniqueArray("Growth");
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetGrowthBuff();
		}
		GrowthAffects();
	}
	else if (playerItems[valueToCheck].Contains("Slowmo") == true)
	{
		slowItemCounter++;
		UpdateUniqueArray("Slowmo");
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetSlowBuff();
		}
		SlowAffects();
	}
	else if (playerItems[valueToCheck].Contains("Hyper") == true)
	{
		hyperItemCounter++;
		UpdateUniqueArray("Hyper");
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetHyperBuff();
		}
		HyperAffects();
	}
	else if (playerItems[valueToCheck].Contains("Money Shot") == true)
	{
		moneyShotItemCounter++;
		UpdateUniqueArray("Money Shot");
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetMnyShotBuff();
		}
		MoneyShotAffects();
	}

	if (uniqueClasses.Num() >= 3)			//tracker for unique classes
	{
		activeDebuffs[0] = true;			//when there are 3+ unique classes activate debuff 1
		if (uniqueClasses.Num() >= 6)	
		{
			activeDebuffs[1] = true;		//when there are 6+ unique classes activate debuff 2

			if (uniqueClasses.Num() >= 9)
			{
				activeDebuffs[2] = true;	//when there are 9+ unique classes activate debuff 3
				DebuffAffects();			//apply debuffs
			}
			else
			{
				DebuffAffects();			//apply debuffs
			}
		}
		else
		{
			DebuffAffects();				//apply debuffs
		}
	}
}

void AItemManager::UpdateUniqueArray(FString itemString)			//keeps track of number of unique classes
{
	bool isNewClass = true;	//assumes a class is new

	if (uniqueClasses.Num() != 0)	//only check if the array has at least 1 value
	{
		for (int i = 0; i < uniqueClasses.Num(); i++) //check against every item of array
		{
			if (uniqueClasses[i].Contains(itemString) == true)	//check if item matches a current item
			{
				isNewClass = false;	//if it does then it is not new
				break;
			}
		}
	}
	if (isNewClass == true)
	{
		uniqueClasses.AddUnique(itemString); //if all are checked and it matches none it is new so add it
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Functions for each family of affects, keep other two functions clean/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::StrAffects()
{
	isABuff = true; //identifies if it is a buff or debuff

	if (strItemCounter == 1)	//values pass increment with counter to some degree
	{
		damageMultiplier = 1.2;	
	}
	else if (strItemCounter == 2)
	{
		damageMultiplier = 1.25;
	}
	else if (strItemCounter == 3)
	{
		damageMultiplier = 2;
		isStrMaxed = true; //acknowledge when maxed
	}
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->ModifyPlayerDamage(isABuff, damageMultiplier); //call to apply to player
	}
}
void AItemManager::MassesAffects()
{
	isABuff = true; //same as above

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
	isABuff = true; //same as above

	if (vigItemCounter == 1)
	{
		healthIncrease = 25;
	}
	else if (vigItemCounter == 2)
	{
		healthIncrease = 25;
	}
	else if (vigItemCounter == 3)
	{
		healthIncrease = 50;
		isHealthRegening = true; //says if health should be regening each time a room is completed
		regenRate = 2;			 //health per room
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
	isABuff = true;	//these functions all follow a similar pattern of design

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
void AItemManager::RateAffects()
{
	isABuff = true; //continues...

	if (rateItemCounter == 1)
	{
		fireRate = 0.25;
	}
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->ModifyPlayerFireRate(isABuff, fireRate);
	}
}
void AItemManager::GrowthAffects()
{
	isABuff = true; //continues...

	if (growthItemCounter == 1)
	{
		initialSpeed = 175;
		maxSpeed = 175;
		lifeSpan = 2.5;
		isGrowing = true;
		scale = FVector(0.5, 0.5, 1);
	}
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->ModifyPlayerProjectileStyle(isABuff, initialSpeed, maxSpeed, lifeSpan, isGrowing, scale);
	}
}
void AItemManager::SlowAffects()
{
	isABuff = true; //continues...

	if (slowItemCounter == 1)
	{
		speedReductionPercent = 0.75;
	}
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->ModifyEnemyMoveSpeed(isABuff, speedReductionPercent);
	}
}
void AItemManager::HyperAffects()
{
	if (hyperItemCounter == 1) //continues...
	{
		percentDamageIncrease = 0.3;
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->ModifyHyperModePercent(percentDamageIncrease);
		}
	}
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->ActivateHyperMode();
	}
}
void AItemManager::MoneyShotAffects()
{
	isABuff = true; //continues...

	if (moneyShotItemCounter == 1)
	{
		dropChanceIncrease = 2;
	}
	for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
		ActorItr->ModifyMoneyDropChance(isABuff, dropChanceIncrease);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions for debuffs and negative affects///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::DebuffAffects()
{
	isABuff = false; //set it is a debuff
	if (activeDebuffs[0] == true)	//applying debuffs based on debuff values
	{
		if (appliedDebuffs[0] != true)	//update applied array is added to + checked as they are applied
		{
			appliedDebuffs[0] = true; //update
			damageMultiplier = 0.75;
			for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
				ActorItr->ModifyPlayerDamage(isABuff, damageMultiplier); //apply
			}
		}
	}
	if (activeDebuffs[1] == true)
	{
		if (appliedDebuffs[1] != true) //do the same for all tier of debuffs
		{
			appliedDebuffs[1] = true;
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
	}
	if (activeDebuffs[2] == true)
	{
		if (appliedDebuffs[2] != true)	//final tier currently
		{
			appliedDebuffs[2] = true;
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
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions for getting world values///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::GetLevelNumber()		//get the level number player is on
{
	FString levelName = GetWorld()->GetMapName();;		//get it via map name - maps are named accordinly
	FString levelNameLeft;			//for splitting the name to get number
	FString levelNameRight;
	FString splitCon = "-";		//names use "-" as a delimiter

	levelName.Split(splitCon, &levelNameLeft, &levelNameRight, ESearchCase::CaseSensitive, ESearchDir::FromStart); //split name

	levelNumber = FCString::Atoi(*levelNameRight);	//set level number as right side of split
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Level transition function////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::TransitionToNewLevel()			//when moving to next level
{
	for (int i = 0; i < currentAvailableItems.Num(); i++)	//check current items available but not taken
	{
		if (currentAvailableItems[i] != itemPool[0])	//check they are not blank values
		{
			itemPool.Add(currentAvailableItems[i]);		//if they are not then add back to item pool
		}
	}
	SetGameInstanceVariables();			//pass consistant values to game instance
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Public GET and SET functions/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TArray<FString> AItemManager::GetCurrentItems()
{
	return (currentAvailableItems); //passes the current available items
}
bool AItemManager::IsStrBuffMaxed()
{
	return isStrMaxed;	//passes has all str buffs
}
bool AItemManager::IsMassBuffMaxed()
{
	return isMassMaxed;	//mass version
}
bool AItemManager::IsVigBuffMaxed()
{
	return isVigMaxed; //vig version
}
void AItemManager::SetGameInstanceVariables()
{
	UDungeonsNDwellingsInstance* GI = Cast<UDungeonsNDwellingsInstance>(GetGameInstance()); //pass all values to game instance that must be maintained between levels
	if (GI)
	{
		GI->GI_itemPool = itemPool;
		GI->GI_playerItems = playerItems;
		GI->GI_uniqueClasses = uniqueClasses;

		GI->GI_strItemCounter = strItemCounter;
		GI->GI_massItemCounter = massItemCounter;
		GI->GI_vigItemCounter = vigItemCounter;
		GI->GI_sacItemCounter = sacItemCounter;
		GI->GI_rateItemCounter = rateItemCounter;
		GI->GI_growthItemCounter = growthItemCounter;
		GI->GI_slowItemCounter = slowItemCounter;
		GI->GI_hyperItemCounter = hyperItemCounter;
		GI->GI_moneyShotItemCounter = moneyShotItemCounter;

		GI->GI_isStrMaxed = isStrMaxed;
		GI->GI_isMassMaxed = isMassMaxed;
		GI->GI_isVigMaxed = isVigMaxed;

		GI->GI_activeDebuffs = activeDebuffs;
		GI->GI_appliedDebuffs = appliedDebuffs;
	}
}

void AItemManager::GetItemStatsFromGI()
{
	UDungeonsNDwellingsInstance* GI = Cast<UDungeonsNDwellingsInstance>(GetGameInstance()); //get all values from instance at level start
	if (levelNumber != 1)
	{
		if (GI)
		{
			itemPool = GI->GI_itemPool;
			playerItems = GI->GI_playerItems;
			uniqueClasses = GI->GI_uniqueClasses;

			strItemCounter = GI->GI_strItemCounter;
			massItemCounter = GI->GI_massItemCounter;
			vigItemCounter = GI->GI_vigItemCounter;
			sacItemCounter = GI->GI_sacItemCounter;
			rateItemCounter = GI->GI_rateItemCounter;
			growthItemCounter = GI->GI_growthItemCounter;
			slowItemCounter = GI->GI_slowItemCounter;
			hyperItemCounter = GI->GI_hyperItemCounter;
			moneyShotItemCounter = GI->GI_moneyShotItemCounter;

			isStrMaxed = GI->GI_isStrMaxed;
			isMassMaxed = GI->GI_isMassMaxed;
			isVigMaxed = GI->GI_isVigMaxed;

			activeDebuffs = GI->GI_activeDebuffs;
			appliedDebuffs = GI->GI_appliedDebuffs;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

