// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicSlugEnemy.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"

// Sets default values
ABasicSlugEnemy::ABasicSlugEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>DoorSealAsset(TEXT("/Game/TwinStick/Meshes/Cylinder.Cylinder"));

	// Create mesh component for the projectile sphere
	CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh0"));
	RootComponent = CylinderMesh;
	CylinderMesh->SetStaticMesh(DoorSealAsset.Object);
	CylinderMesh->SetupAttachment(RootComponent);
	CylinderMesh->BodyInstance.SetCollisionProfileName("SlugEnemy");

	playerLocation = FVector(0, 0, 0);

	slugLocation = FVector(0, 0, 0);
	slugRotation = FRotator(0, 0, 0);
	slugScale = FVector(0, 0, 0);
	isEnemyActive = false;

	moveSpeed = 8;
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
