// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyProjectile.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS(config = Game)
class AEnemyProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* ProjectileMesh;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* ProjectileMovement;

public:
	AEnemyProjectile();

	/** Function to handle the projectile hitting something */
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns ProjectileMesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return ProjectileMesh; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
		UMaterialInterface* theMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
		UMaterialInstanceDynamic* dynamicMaterial;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	/**********************************************************************PUBLIC VARIABLES***************************************************************************/

	FString bossName;
	float bossDmg;


	void UpdateMaterials();
};
