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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EnemyProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/EnemyProjectile.EnemyProjectile")); //attach mesh asset

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EPMesh0"));  //mesh object for projectile
	ProjectileMesh->SetStaticMesh(EnemyProjectileMeshAsset.Object);				//attach to asset
	//ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("EnemyProjectile");			//setup collision
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;				//setup root
		
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("EPMovement0"));		//movement component
	ProjectileMovement->UpdatedComponent = ProjectileMesh;				
	ProjectileMovement->bRotationFollowsVelocity = true;						//constant velocity
	ProjectileMovement->bShouldBounce = false;									//no bouncing
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	//Mobility
	ProjectileMesh->SetMobility(EComponentMobility::Movable);			

	//sets material component
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> Material(TEXT("/Game/TwinStick/Meshes/YellowMaterial.YellowMaterial"));		//helper for changeable material
	if (Material.Object != NULL)
	{
		theMaterial = (UMaterialInstanceConstant*)Material.Object;		//material object
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
		bossDmg = ActorItr->GetCurrentBossDmg(); //get the current bosses damage
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control enemy projectile stats and effects///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString actorName = OtherActor->GetName(); //get name of actor hit

	if (actorName == "TP_TwinStickPawn_1") //is it the player
	{
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			ActorItr->PlayerTakeDamage(bossDmg);	//if yes player takes damage
		}
	}
	Destroy();	//destroy projectile
}
void AEnemyProjectile::UpdateMaterials()
{
	dynamicMaterial = UMaterialInstanceDynamic::Create(theMaterial, this);	//update to new material as called for
	ProjectileMesh->SetMaterial(0, dynamicMaterial);	//set material
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
