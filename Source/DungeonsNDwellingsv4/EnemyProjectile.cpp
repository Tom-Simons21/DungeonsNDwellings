// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"

// Sets default values
AEnemyProjectile::AEnemyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EnemyProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/EnemyProjectile.EnemyProjectile"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EPMesh0"));
	ProjectileMesh->SetStaticMesh(EnemyProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("EnemyProjectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("EPMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	//Mobility
	ProjectileMesh->SetMobility(EComponentMobility::Movable);

	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> Material(TEXT("/Game/TwinStick/Meshes/YellowMaterial.YellowMaterial"));
	if (Material.Object != NULL)
	{
		theMaterial = (UMaterialInstanceConstant*)Material.Object;
	}


	ProjectileMovement->InitialSpeed = 300;
	ProjectileMovement->MaxSpeed = 300;
	InitialLifeSpan = 4;
}

void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
	}
	Destroy();
}

void AEnemyProjectile::UpdateMaterials()
{
	dynamicMaterial = UMaterialInstanceDynamic::Create(theMaterial, this);
	ProjectileMesh->SetMaterial(0, dynamicMaterial);
}

