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
	//////////////////////////////////////////////////////////////////////////////////////////////////////


	//Projectile Stats/////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		float initialSpeed;
	UPROPERTY(EditAnywhere)
		float maxSpeed;
	UPROPERTY(EditAnywhere)
		float lifeSpan;
	UPROPERTY(EditAnywhere)
		float projectileDamage;
	///////////////////////////////////////////////////////////////////////////////////////////////////


	//Key vector locations///////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		FVector playerZElevation;

	UPROPERTY(EditAnywhere)
		FVector roomPlacementModifier;

	FVector playerStartPoint = FVector(175, 400, 22);
	FVector playerScale = FVector(0.4, 0.4, 0.4);
	/////////////////////////////////////////////////////////////////////////////////////////////////


	//Timers//////////////////////////////////////////////////////////////////////////////////////////
	FTimerHandle hitControlTimer;
	//////////////////////////////////////////////////////////////////////////////////////////////////


	//Door Modifier + Tracker Variables used for player move algorithms///////////////////////////////
	UPROPERTY(EditAnywhere)
		int totalDoorNum;
	UPROPERTY(EditAnywhere)
		TArray<int32> arrayOfDoors;
	UPROPERTY(EditAnywhere)
		TArray<FVector> doorStartPoints;
	UPROPERTY(EditAnywhere)
		TArray<FVector> doorEndPoints;
	UPROPERTY(EditAnywhere)
		FVector playerExitLoc;
	UPROPERTY(EditAnywhere)
		bool isNewDoor;
	///////////////////////////////////////////////////////////////////////////////////////////////////





	/*************************************************************************PUBLIC FUNCTIONS***************************************************************************/

	//Functions for getting and passing key variables to other classes////////////////////////////////////////////////
	void getTotalOfDoors();
	FVector getCurrentLocation();							//can be called by other objects to get player location
	void GetRoomPlacementModifier();
	FVector GetPlayerZOffset();
	void createArrayOfDoors();
	float GetProjectileDamage();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions for updating variables relative to projectile/////////////////////////////////////////////////////////
	float updateProperties(float defaultVal);
	void updateProjectileValues(float initSpeed, float maxSpeed, float life);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions for controlling player damage taken///////////////////////////////////////////////////////////////////
	void makeDamageable();
	void setIsDamageable(bool isDamageable);
	void takeDamage(float damageToTake);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Functions for tracking player movement through level and current location///////////////////////////////////////
	void getPlayerLocation();										//passes player location to other objects
	void moveToRoom(FVector zLoc, FVector doorLocation);
	void checkPlayerLocation(FVector playLoc, FVector zLoc);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Functions to control player interactions and additional actions/////////////////////////////////////////////////
	void OnInteract();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

