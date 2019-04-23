// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicSlugEnemy.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "DungeonsNDwellingsv4Projectile.h"
#include "EnemySpawner.h"
#include "TileGeneratorParent.h"

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

	//keep track of the players location so we can move towards them
	playerLocation = FVector(0, 0, 0);

	//current generic values incase we need to more directly modify enemy transform
	slugLocation = FVector(0, 0, 0);
	slugRotation = FRotator(0, 0, 0);
	slugScale = FVector(0, 0, 0);
	//enemies start in the "off" state where they will not call their respective tick functions
	isEnemyActive = false;

	//default values for this specific enemy type
	moveSpeed = 90;
	slugHealth = 40;
	slugDamage = 15;
}


//Functions to control the basic functionality of the slug enemy object///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
void ABasicSlugEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetRoomPlacementModifier();
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

//Checks if the slug enemy has been hit, this function should be present on all enemy objects, 
//effects of this type are being managed on the player + enemy as this combination is less 
//volatile as all processes can complete before destroy
void ABasicSlugEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString actorName;
	float playerDmg;
	float moveSpeedModifier;

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
			OtherActor->Destroy();

			for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
				playerDmg = ActorItr->GetProjectileDamage();
				moveSpeedModifier = ActorItr->GetMoveSpeedModifier();
			}
			this->SetSlugMoveSpeed(moveSpeedModifier);
			this->takeDamage(playerDmg);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control the behaviour of the slug enemies/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ABasicSlugEnemy::setIsEnemyActive()
{
	isEnemyActive = true;
	GetWorld()->GetTimerManager().SetTimer(moveSpeed_TimerHandle, this, &ABasicSlugEnemy::MaintainSlugMovementSpeed, 1.f, true, 0.f);
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

void ABasicSlugEnemy::MaintainSlugMovementSpeed()
{
	moveSpeed = 90;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control slug enemies taking damage and the application of status effects////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ABasicSlugEnemy::takeDamage(float dmg)
{
	float zLoc = GetZLocation();
	FString enemyName = this->GetName();
	int roomNumber = (zLoc - slugZOffset) / roomPlacementModifier.Z;

	slugHealth -= dmg;

	if (slugHealth <= 0)
	{
		for (TActorIterator<AEnemySpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AEnemySpawner *Object = *ActorItr;
			ActorItr->checkRoomCleared(roomNumber);
			ActorItr->removeArrayItem(enemyName);
		}
		Destroy();
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to GET and SET key variables between classes//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ABasicSlugEnemy::GetZLocation()
{
	FVector loc = GetActorLocation();
	float zLoc = loc.Z;
	return (zLoc);
}

float ABasicSlugEnemy::GetZOffset()
{
	return (slugZOffset);
}

void ABasicSlugEnemy::GetRoomPlacementModifier()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->getRoomPlacementModifier();
	}
}

void ABasicSlugEnemy::SetSlugMoveSpeed(float speedModifier)
{
	moveSpeed = moveSpeed * speedModifier;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
