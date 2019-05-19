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

	/**********************************************************************PRIVATE VARIABLES*******************************************************************************/

	//Player Stats/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		bool isDamageable;			//can the player be damaged
	UPROPERTY(EditAnywhere)
		float playerHealth;				//amount of health
	UPROPERTY(EditAnywhere)
		float playerHealthDefault;			//starting health value
	UPROPERTY(EditAnyWhere)
		float playerMaxHealth;			//maximum health the can be reached
	UPROPERTY(EditAnywhere)
		int playerGold;					//player currency
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Projectile Stats////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		float initialSpeed;				//all of these are described on projectile
	UPROPERTY(EditAnywhere)
		float maxSpeed;
	UPROPERTY(EditAnywhere)
		float lifeSpan;
	UPROPERTY(EditAnywhere)
		bool isGrowing;
	UPROPERTY(EditAnywhere)
		FVector projectileScale;
	//Local projectile modifiers/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		float projectileDamage;					//damage projectile deals
	UPROPERTY(EditAnywhere)
		float projectileDefaultDamage;			//starting projectile damage
	bool isSpawningSecondShot;					//check if should be spawning a second shot
	bool isHealthDraining;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Currency variables/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int goldToAdd;					//amount of gold to gain
	int winStreak;					//bonus to earn for win streak
	int loseStreak;					//bonus to earn for lose streak
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Key vector locations///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FVector playerZElevation;												//distance off the floor
	FVector roomPlacementModifier;											//distance between rooms
	TArray<float> uniqueZTracker = { 0 };									//track unique rooms the player has been in
	FVector playerStartPoint = FVector(175, 400, 22);						//player spawn point
	FVector playerScale = FVector(0.4, 0.4, 0.4);							//player scale
	FVector exitPoint;														//the point the player will be placed at when leaving a room
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Timers/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FTimerHandle hitControlTimer;   //resets the players "damageable" stat every x seconds - provides temp invulnerability on hit
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Pause Menu/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool isPaused;				//is the game paused
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Door Modifier + Tracker Variables used for player move algorithms//////////////////////////////////////////////////////////////////////////////////////////////////
	TArray<FVector> doorMapping;					//array to map doors together
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Variables for player stats activation//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool strBuffActive;
	bool massBuffActive;			//active buffs
	bool vigBuffActive;
	bool sacBuffActive;
	bool rateBuffActive;
	bool growthBuffActive;
	bool slowBuffActive;
	bool hyperBuffActive;
	bool mnyShotBuffActive;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Player stats provided only via items///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		float healthFromKills;				//amount of health to get from a kill
	UPROPERTY(EditAnywhere)		
		int chanceToRecieveHealth;			//chance to recieve the health from a kill
	UPROPERTY(EditAnywhere)
		int spawnChanceValue;				//chance to spawn a second projectile
	UPROPERTY(EditAnywhere)
		float healthRegenValue;				//amount of health to regen
	UPROPERTY(EditAnywhere)
		float enemySpeedReductionPercent;	//amount of speed modifier to apply to enemy
	UPROPERTY(EditAnywhere)
		float otherDmgChanges;				//tracks changes to player damage
	UPROPERTY(EditAnywhere)
		float lastHyperModeChange;			//tracks hyper mode
	UPROPERTY(EditAnywhere)
		float hyperModePercent;				//tracks hyper mode values
	UPROPERTY(EditAnywhere)
		float moneyDropModifier;			//change chance for money to drop
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Variables for world stats//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FString const levelPrefix = "DwellingsMap-";			//default level prefix, this should be used when naming all levels
	int levelNumber;										//will hold the number that follows the prefix
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Variables from external classes////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int roomCount;						//number of rooms
	bool isLevelComplete;				//has the boss been killed
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	/** Returns PlayerMeshComponent subobject **/
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return PlayerMeshComponent; }
	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }


	/*************************************************************************PUBLIC FUNCTIONS***************************************************************************/

	//Functions for updating variables relative to projectile////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void UpdateProjectileValues(float initSpeed, float maxSpeed, float life, bool isGrowing, FVector scale);		//update values for projectile
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions for controlling player damage taken//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void MakeDamageable();									//make the player damageable
	void ApplyInvulnerability(bool isDamageable);			//make invulnerable
	void PlayerTakeDamage(float damageToTake);				//deal damage to player
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions for tracking player movement through level and current location//////////////////////////////////////////////////////////////////////////////////////////
	void CalcPlayerLocation();										//passes player location to other objects
	void CheckPlayerLocation();										//checks if a player is stood in a door way
	void SetLocationAdjustment(FVector doorLocation);				//sets player location to dead centre when going through a door, this is the position used to compare with array values
	void MoveToRoom(FVector newLocation);							//moves player to the corresponding door in the corresponding room
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions to control player interactions and additional actions////////////////////////////////////////////////////////////////////////////////////////////////////
	void OnInteract();						//player takes item
	void OnReroll();						//player rerolls item
	void OnPauseGame();						//player presses pause game
	void OnNextLevel();						//player presses next level
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions to control player currency earning/spending/interest/////////////////////////////////////////////////////////////////////////////////////////////////////
	void UpdatePlayerCurrency();			//change player currency value
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions to apply item affects to players/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SetStrBuff();				
	void SetMassBuff();				//set buffs to be active etc
	void SetVigBuff();
	void SetSacBuff();
	void SetRateBuff();
	void SetGrowthBuff();
	void SetSlowBuff();
	void SetHyperBuff();
	void SetMnyShotBuff();

	void ModifyPlayerDamage(bool isABuff, float damageMultiplier);				//changes player damage
	void ModifyProjectileSpawnChance(bool isABuff, int spawnChanceModifier);	//changes spawn chance for second projectile
	void ModifyPlayerHealth(bool isABuff, float healthIncrease, bool isHealthRegening, float healthRegenAmount);		//changes player health
	void ModifyPlayerKillBonuses(bool isABuff, float healthToRecieve, int chanceToRecieve);					//activates/sets values for bonuses on enemy kill
	void ModifyPlayerFireRate(bool isABuff, float fireRate);										//modifies fire rate
	void ModifyPlayerProjectileStyle(bool isABuff, float initSpeed, float topSpeed, float lifeTime, bool isGrow, FVector scale);	//modifies projectile values
	void ModifyEnemyMoveSpeed(bool isABuff, float reductionPercent);			//sets values for applying speed debuff to enemies
	void ModifyMoneyDropChance(bool isABuff, float dropChanceIncrease);				//sets values to modify money drop rate
	void ModifyHyperModePercent(float hyperPercent);					//sets hyper mode values

	bool SpawnAdditionalShots(FVector FireDirection);	//spawns an additional shot
	void RegenHealth();									//gives health back to player on successful progression through room
	void GainHealthOnKill();							//gives player health back on enemy kill
	void ActivateHyperMode();							//applies hyper mode effects
	void SetDrainHealth();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions for world variables////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void GetLevelNumber();								//gets the number of the current level
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions for getting and passing key variables to other classes/////////////////////////////////////////////////////////////////////////////////////////////////
	FVector GetPlayersCurrentLocation();							//can be called by other objects to get player location
	void GetRoomPlacementModifier();								//gets distance between rooms
	FVector GetPlayerZOffset();										//gets player distance off of floor
	float GetProjectileDamage();									//gets the damage the projectile should do
	void GetRoomCount();											//gets number of rooms
	float GetMoveSpeedModifier();									//gets the amount to modify enemy speed by
	void GetDoorMappings();											//gets the array that maps doors together
	void SetLevelComplete();										//sets the level to be complete
	void SetGameInstanceVariables();								//passes all consistent variables to the game instance
	void GetPlayerStatsFromGI();									//gets all variables back from the game instance

	UFUNCTION(BlueprintPure, Category = "PlayerHealth")
		float GetPlayerHealth();									//passes player health to the hud
	UFUNCTION(BlueprintPure, Category = "PlayerGold")
		int GetPlayerCurrency();									//passes player currency to the hud 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

