// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "BossManager.h"

// Sets default values
AEnemyProjectile::AEnemyProjectile()
{
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

	//sets material component
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> Material(TEXT("/Game/TwinStick/Meshes/YellowMaterial.YellowMaterial"));
	if (Material.Object != NULL)
	{
		theMaterial = (UMaterialInstanceConstant*)Material.Object;
	}

	//sets projectile stats
	ProjectileMovement->InitialSpeed = 300;
	ProjectileMovement->MaxSpeed = 300;
	InitialLifeSpan = 4;
}

//gets the damage on begin play - this means the projectile stats can be altered per boss///////////////////////////////////////////////////////////////////////////////////////////////////
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<ABossManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ABossManager *Object = *ActorItr;
		bossDmg = ActorItr->GetCurrentBossDmg();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control enemy projectile stats and effects///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString actorName = OtherActor->GetName();

	if (actorName == "TP_TwinStickPawn_1")
	{
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->PlayerTakeDamage(bossDmg);
		}
	}
	Destroy();
}
void AEnemyProjectile::UpdateMaterials()
{
	dynamicMaterial = UMaterialInstanceDynamic::Create(theMaterial, this);
	ProjectileMesh->SetMaterial(0, dynamicMaterial);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
