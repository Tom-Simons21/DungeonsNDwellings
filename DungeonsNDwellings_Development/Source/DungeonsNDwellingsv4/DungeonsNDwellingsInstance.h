// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DungeonsNDwellingsInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONSNDWELLINGSV4_API UDungeonsNDwellingsInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	//Variables to maintain for player////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float GI_playerHealth;						
	float GI_playerMaxHealth;
	int GI_playerGold;
	float GI_initialSpeed;
	float GI_maxSpeed;
	float GI_lifeSpan;
	float GI_fireRate;
	bool GI_isGrowing;
	FVector GI_projectileScale;
	float GI_projectileDamage;
	bool GI_isSpawningSecondShot;

	bool GI_strBuffActive;
	bool GI_massBuffActive;
	bool GI_vigBuffActive;
	bool GI_sacBuffActive;
	bool GI_rateBuffActive;
	bool GI_growthBuffActive;
	bool GI_slowBuffActive;
	bool GI_hyperBuffActive;
	bool GI_mnyShotBuffActive;

	float GI_healthFromKills;
	int GI_chanceToRecieveHealth;

	int GI_spawnChanceValue;
	float GI_healthRegenValue;

	float GI_enemySpeedReductionPercent;

	float GI_otherDmgChanges;

	float GI_moneyDropModifier;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	//Variables to maintain the item manager//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TArray<FString> GI_itemPool;
	TArray<FString> GI_playerItems;
	TArray<FString> GI_uniqueClasses;

	int GI_strItemCounter;
	int GI_massItemCounter;
	int GI_vigItemCounter;
	int GI_sacItemCounter;
	int GI_rateItemCounter;
	int GI_growthItemCounter;
	int GI_slowItemCounter;
	int GI_hyperItemCounter;
	int GI_moneyShotItemCounter;

	bool GI_isStrMaxed;
	bool GI_isMassMaxed;
	bool GI_isVigMaxed;

	TArray<bool> GI_activeDebuffs;
	TArray<bool> GI_appliedDebuffs;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
