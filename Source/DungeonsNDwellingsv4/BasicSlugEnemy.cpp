// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicSlugEnemy.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "EnemySpawner.h"

// Sets default values
ABasicSlugEnemy::ABasicSlugEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>CylinderMesh(TEXT("/Game/TwinStick/Meshes/Cylinder.Cylinder"));

	// Create mesh component for the projectile sphere
	CylinderMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh0"));
	RootComponent = CylinderMeshComponent;
	CylinderMeshComponent->SetStaticMesh(CylinderMesh.Object);
	CylinderMeshComponent->SetupAttachment(RootComponent);
	CylinderMeshComponent->BodyInstance.SetCollisionProfileName("BlockAllDynamic");

	CylinderMeshComponent->SetSimulatePhysics(true);
	CylinderMeshComponent->SetLinearDamping(1000);
	CylinderMeshComponent->SetAngularDamping(1000);
	CylinderMeshComponent->SetEnableGravity(false);
	CylinderMeshComponent->SetConstraintMode(EDOFMode::XYPlane);
	CylinderMeshComponent->SetMassOverrideInKg(NAME_None, 1000);
	CylinderMeshComponent->bIgnoreRadialImpulse = true;
	CylinderMeshComponent->bIgnoreRadialForce = true;
	CylinderMeshComponent->bApplyImpulseOnDamage = false;
	CylinderMeshComponent->bReplicatePhysicsToAutonomousProxy = false;

	CylinderMeshComponent->SetNotifyRigidBodyCollision(true);
	CylinderMeshComponent->OnComponentHit.AddDynamic(this, &ABasicSlugEnemy::OnHit);	//set up a notification for when this component hits something

	playerLocation = FVector(0, 0, 0);

	slugLocation = FVector(0, 0, 0);
	slugRotation = FRotator(0, 0, 0);
	slugScale = FVector(0, 0, 0);
	isEnemyActive = false;

	moveSpeed = 8;
	slugHealth = 40;
	slugDamage = 10;
}

// Called when the game starts or when spawned
void ABasicSlugEnemy::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABasicSlugEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isEnemyActive == true)
	{
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			playerLocation = ActorItr->getCurrentLocation();
		}

		moveTowardsPlayer(DeltaTime);
	}
}

void ABasicSlugEnemy::setIsEnemyActive()
{
	isEnemyActive = true;
}

void ABasicSlugEnemy::moveTowardsPlayer(float deltaTime)
{
	FVector myLocation = GetActorLocation();
	FVector distanceToPlayer = myLocation - playerLocation;
	distanceToPlayer.Normalize();

	FVector velocity = (distanceToPlayer * moveSpeed * deltaTime) * (-1);

	velocity.Z = 0;


	SetActorLocation(myLocation + velocity);
}

float ABasicSlugEnemy::getZLocation()
{
	FVector loc = GetActorLocation();

	float zLoc = loc.Z;

	return (zLoc);
}

void ABasicSlugEnemy::takeDamage(float dmg)
{
	float zLoc = getZLocation();
	FString enemyName = this->GetName();
	int roomNumber = (zLoc - 61) / 2000;

	slugHealth -= dmg;

	if (slugHealth <= 0)
	{
		for (TActorIterator<AEnemySpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AEnemySpawner *Object = *ActorItr;
			ActorItr->checkRoomCleared(roomNumber);
		}
		for (TActorIterator<AEnemySpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AEnemySpawner *Object = *ActorItr;
			ActorItr->removeArrayItem(enemyName);
		}
		
		Destroy();
	}
}

void ABasicSlugEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString actorName;
	float playerDmg;

	// Create a damage event  
	TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
	FDamageEvent DamageEvent(ValidDamageTypeClass);

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		actorName = *OtherActor->GetName();

		if (actorName == "TP_TwinStickPawn_1")
		{
			for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
				ActorItr->takeDamage(slugDamage);
			}
		}
		else if (actorName.Contains("DungeonsNDwellingsv4Projectile_"))
		{
			for (TActorIterator<AEnemySpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				AEnemySpawner *Object = *ActorItr;
				playerDmg = ActorItr->getPlayerCurrentDmg();
			}

			this->takeDamage(playerDmg);
		}
	}
}
