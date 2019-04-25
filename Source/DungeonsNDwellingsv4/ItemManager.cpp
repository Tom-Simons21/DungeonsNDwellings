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
	availableItemsCounter = 0;
	strItemCounter = 0;
	massItemCounter = 0;
	vigItemCounter = 0;
	sacItemCounter = 0;
	rateItemCounter = 0;
	growthItemCounter = 0;
	slowItemCounter = 0;
	hyperItemCounter = 0;
	moneyShotItemCounter = 0;

	//on start no buffs should be maxed
	isStrMaxed = false;
	isMassMaxed = false;
	isVigMaxed = false;

	//initialise all variables prior to ensure all are initialised
	damageMultiplier = 1;
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
	activeDebuffs = { false, false, false };
	currentAvailableItems.Empty();
	playerItems.Empty();
	uniqueClasses.Empty();
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

//Controlling functionality of player item interaction, setting items on reroll, setting items to player array on take/////////////////////////////////////////////////////////////////////////////////////////////
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
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control and apply affects to players based off of item selection////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::TrackAffects()
{
	int valueToCheck = (playerItems.Num() - 1); //check the last item added

	if (playerItems[valueToCheck].Contains("Strength") == true)
	{
		strItemCounter++;
		UpdateUniqueArray("Strength");
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->SetStrBuff();
		}
		StrAffects();
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

void AItemManager::UpdateUniqueArray(FString itemString)
{
	bool isNewClass = true;

	if (uniqueClasses.Num() != 0)
	{
		for (int i = 0; i < uniqueClasses.Num(); i++)
		{
			if (uniqueClasses[i].Contains(itemString) == true)
			{
				isNewClass = false;
				break;
			}
		}
	}
	if (isNewClass == true)
	{
		uniqueClasses.AddUnique(itemString);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Functions for each family of affects, keep other two functions clean/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::StrAffects()
{
	isABuff = true;

	if (strItemCounter == 1)
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
void AItemManager::RateAffects()
{
	isABuff = true;

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
	isABuff = true;

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
	isABuff = true;

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
	if (hyperItemCounter == 1)
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
	isABuff = true;

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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions for getting world values///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::GetLevelNumber()
{
	FString levelName = GetWorld()->GetMapName();;
	FString levelNameLeft;
	FString levelNameRight;
	FString splitCon = "-";

	levelName.Split(splitCon, &levelNameLeft, &levelNameRight, ESearchCase::CaseSensitive, ESearchDir::FromStart);

	levelNumber = FCString::Atoi(*levelNameRight);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Level transition function////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AItemManager::TransitionToNewLevel()
{
	for (int i = 0; i < currentAvailableItems.Num(); i++)
	{
		if (currentAvailableItems[i] != itemPool[0])
		{
			itemPool.Add(currentAvailableItems[i]);
		}
	}
	SetGameInstanceVariables();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Public GET and SET functions/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
void AItemManager::SetGameInstanceVariables()
{
	UDungeonsNDwellingsInstance* GI = Cast<UDungeonsNDwellingsInstance>(GetGameInstance());
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
	}
}

void AItemManager::GetItemStatsFromGI()
{
	UDungeonsNDwellingsInstance* GI = Cast<UDungeonsNDwellingsInstance>(GetGameInstance());
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
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

