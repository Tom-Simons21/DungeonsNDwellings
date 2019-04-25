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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("PlayerProjectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ADungeonsNDwellingsv4Projectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;

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
	Destroy();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function to modify statistics and behaviours of the projectile////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Projectile::UpdateProperties(float speedStart, float speedMax, float projLife, bool isGrowing, FVector scale)
{
	ProjectileMovement->InitialSpeed = speedStart;
	ProjectileMovement->MaxSpeed = speedMax;
	InitialLifeSpan = projLife;
	isProjectileGrowing = isGrowing;
	projectileScale = scale;

	if (isProjectileGrowing == true)
	{
		GetWorldTimerManager().SetTimer(growthTimer, this, &ADungeonsNDwellingsv4Projectile::GrowProjectile, 0.25f, true, 0.f);
	}
}
void ADungeonsNDwellingsv4Projectile::GrowProjectile()
{
	projectileScale += FVector(0.35, 0.35, 0);
	this->SetActorScale3D(projectileScale);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
