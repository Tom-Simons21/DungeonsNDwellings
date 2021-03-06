// Copyright 1998-2018 Epic Games, Inc. All Rights Reserve

#include "DungeonsNDwellingsv4Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"

ADungeonsNDwellingsv4Projectile::ADungeonsNDwellingsv4Projectile() 
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile")); //set the asset to use
	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0")); //create mesh object
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);		//attach asset to mesh
	//ProjectileMesh->SetupAttachment(RootComponent);
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		ProjectileMesh->BodyInstance.SetCollisionProfileName("PlayerProjectile");	//collision setting
	}
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ADungeonsNDwellingsv4Projectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh; //setup root

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	
	//Modify projectile stats
	isProjectileGrowing = false;
}

//Function destroys projectile on hit///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();	//destroy on hit
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function to modify statistics and behaviours of the projectile////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Projectile::UpdateProperties(float speedStart, float speedMax, float projLife, bool isGrowing, FVector scale)
{
	ProjectileMovement->InitialSpeed = speedStart;		//change move speed
	ProjectileMovement->MaxSpeed = speedMax;			//change max speed
	InitialLifeSpan = projLife;							//change life span
	isProjectileGrowing = isGrowing;					//set is growing
	projectileScale = scale;							//change scale
		
	if (isProjectileGrowing == true)	
	{
		GetWorldTimerManager().SetTimer(growthTimer, this, &ADungeonsNDwellingsv4Projectile::GrowProjectile, 0.25f, true, 0.f); //start timer if it is growing
	}
}
void ADungeonsNDwellingsv4Projectile::GrowProjectile()
{
	projectileScale += FVector(0.35, 0.35, 0);	//scale increases over time
	this->SetActorScale3D(projectileScale);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
