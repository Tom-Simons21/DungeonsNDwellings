// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DungeonsNDwellingsv4Pawn.generated.h"

UCLASS(Blueprintable)
class ADungeonsNDwellingsv4Pawn : public APawn
{
	GENERATED_BODY()

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* PlayerMeshComponent;

	/** The camera */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

public:
	ADungeonsNDwellingsv4Pawn();

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		FVector GunOffset;

	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float FireRate;

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float MoveSpeed;

	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
		class USoundBase* FireSound;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	/* Fire a shot in the specified direction */
	void FireShot(FVector FireDirection);

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

	void BeginPlay();

	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;

private:

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

public:
	/** Returns PlayerMeshComponent subobject **/
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return PlayerMeshComponent; }
	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }



	/**********************************************************************PUBLIC VARIABLES***************************************************************************/

	//Player Stats/////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		bool isDamageable;
	UPROPERTY(EditAnywhere)
		float playerHealth;
	UPROPERTY(EditAnywhere)
		float playerHealthDefault;
	UPROPERTY(EditAnyWhere)
		float playerMaxHealth;
	UPROPERTY(EditAnywhere)
		int playerGold;
	//////////////////////////////////////////////////////////////////////////////////////////////////////


	//Projectile Stats////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		float initialSpeed;
	UPROPERTY(EditAnywhere)
		float maxSpeed;
	UPROPERTY(EditAnywhere)
		float lifeSpan;
	UPROPERTY(EditAnywhere)
		bool isGrowing;
	UPROPERTY(EditAnywhere)
		FVector projectileScale;

	//Local projectile modifiers/////////////////////////////////////////////////////////////////////////
	/**/UPROPERTY(EditAnywhere)
	/**/	float projectileDamage;
	/**/UPROPERTY(EditAnywhere)
	/**/	float projectileDefaultDamage;
	/**/bool isSpawningSecondShot;
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////


	//Currency variables/////////////////////////////////////////////////////////////////////////////////
	int goldToAdd;
	int winStreak;
	int loseStreak;
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	//Key vector locations///////////////////////////////////////////////////////////////////////////////
	FVector playerZElevation;
	FVector roomPlacementModifier;
	TArray<float> uniqueZTracker = { 0 };

	FVector playerStartPoint = FVector(175, 400, 22);
	FVector playerScale = FVector(0.4, 0.4, 0.4);

	UPROPERTY(EditAnywhere)
		FVector exitPoint;
	/////////////////////////////////////////////////////////////////////////////////////////////////


	//Timers//////////////////////////////////////////////////////////////////////////////////////////
	FTimerHandle hitControlTimer;
	//////////////////////////////////////////////////////////////////////////////////////////////////


	//Pause Menu//////////////////////////////////////////////////////////////////////////////////////
	bool isPaused;
	//////////////////////////////////////////////////////////////////////////////////////////////////


	//Door Modifier + Tracker Variables used for player move algorithms///////////////////////////////
	UPROPERTY(EditAnywhere)
		int totalDoorNum;
	UPROPERTY(EditAnywhere)
		TArray<int32> arrayOfDoors;
	UPROPERTY(EditAnywhere)
		TArray<FVector> doorMapping;
	///////////////////////////////////////////////////////////////////////////////////////////////////

	
	//Variables for player stats activation////////////////////////////////////////////////////////////
	bool strBuffActive;
	bool massBuffActive;
	bool vigBuffActive;
	bool sacBuffActive;
	bool rateBuffActive;
	bool growthBuffActive;
	bool slowBuffActive;
	bool hyperBuffActive;
	bool mnyShotBuffActive;

	UPROPERTY(EditAnywhere)
		float healthFromKills;
	UPROPERTY(EditAnywhere)
		int chanceToRecieveHealth;

	int spawnChanceValue;
	float healthRegenValue;

	UPROPERTY(EditAnywhere)
		float enemySpeedReductionPercent;

	UPROPERTY(EditAnywhere)
		float otherDmgChanges;

	float moneyDropModifier;
	///////////////////////////////////////////////////////////////////////////////////////////////////


	//Variables from external classes//////////////////////////////////////////////////////////////////
	int roomCount;
	FString bossLabel;
	///////////////////////////////////////////////////////////////////////////////////////////////////






	/*************************************************************************PUBLIC FUNCTIONS***************************************************************************/

	//Functions for updating variables relative to projectile/////////////////////////////////////////////////////////
	void UpdateProjectileValues(float initSpeed, float maxSpeed, float life, bool isGrowing, FVector scale);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions for controlling player damage taken///////////////////////////////////////////////////////////////////
	void makeDamageable();
	void setIsDamageable(bool isDamageable);
	void takeDamage(float damageToTake);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions for tracking player movement through level and current location///////////////////////////////////////
	void getPlayerLocation();										//passes player location to other objects
	void moveToRoom(FVector newLocation);
	void checkPlayerLocation(FVector playLoc, FVector zLoc);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions to control player interactions and additional actions/////////////////////////////////////////////////
	void OnInteract();
	void OnReroll();
	void OnPauseGame();
	void OnNextLevel();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions to control player currency earning/spending/interest//////////////////////////////////////////////////
	void UpdatePlayerCurrency();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions to apply item affects to players//////////////////////////////////////////////////////////////////////
	void SetStrBuff();
	void SetMassBuff();
	void SetVigBuff();
	void SetSacBuff();
	void SetRateBuff();
	void SetGrowthBuff();
	void SetSlowBuff();
	void SetHyperBuff();
	void SetMnyShotBuff();

	void ModifyPlayerDamage(bool isABuff, float damageMultiplier);
	void ModifyProjectileSpawnChance(bool isABuff, int spawnChanceModifier);
	void ModifyPlayerHealth(bool isABuff, float healthIncrease, bool isHealthRegening, float healthRegenAmount);
	void ModifyPlayerKillBonuses(bool isABuff, float healthToRecieve, int chanceToRecieve);
	void ModifyPlayerFireRate(bool isABuff, float fireRate);
	void ModifyPlayerProjectileStyle(bool isABuff, float initSpeed, float topSpeed, float lifeTime, bool isGrow, FVector scale);
	void ModifyEnemyMoveSpeed(bool isABuff, float reductionPercent);
	void ModifyMoneyDropChance(bool isABuff, float dropChanceIncrease);

	bool SpawnAdditionalShots(FVector FireDirection);
	void RegenHealth();
	void GainHealthOnKill();
	void ActivateHyperMode(float percentDmgIncrease);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions for getting and passing key variables to other classes////////////////////////////////////////////////
	void getTotalOfDoors();
	FVector getCurrentLocation();							//can be called by other objects to get player location
	void GetRoomPlacementModifier();
	FVector GetPlayerZOffset();
	void createArrayOfDoors();
	float GetProjectileDamage();
	void GetRoomCount();
	float GetMoveSpeedModifier();

	void GetDoorMappings();

	UFUNCTION(BlueprintPure, Category = "PlayerHealth")
		float GetPlayerHealth();

	UFUNCTION(BlueprintPure, Category = "PlayerHealth")
		int GetPlayerCurrency();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

