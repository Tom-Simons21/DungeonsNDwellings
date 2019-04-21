// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonsNDwellingsv4Projectile.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS(config=Game)
class ADungeonsNDwellingsv4Projectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ProjectileMesh;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:
	ADungeonsNDwellingsv4Projectile();

	/** Function to handle the projectile hitting something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns ProjectileMesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return ProjectileMesh; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }


	/**********************************************************************PUBLIC VARIABLES***************************************************************************/

	//Custom properties for the projectile/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float damage;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	/**********************************************************************PUBLIC FUNCTIONS***************************************************************************/
	
	//Update properties on a projectile should be called via a deffered spawn//////////////////////////////////////////////////////////////////////////////////////////
	void updateProperties(float speedStart, float speedMax, float projLife);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

