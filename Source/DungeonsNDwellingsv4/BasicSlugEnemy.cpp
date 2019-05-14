// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicSlugEnemy.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "EnemySpawner.h"
#include "TileGeneratorParent.h"

// Sets default values
ABasicSlugEnemy::ABasicSlugEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>CylinderMesh(TEXT("/Game/TwinStick/Meshes/Cylinder.Cylinder"));	//select mesh to use for enemy

	// Create mesh component for the projectile sphere
	CylinderMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh0"));							//create mesh component
	RootComponent = CylinderMeshComponent;																					//attach to root - still not 100% clear on the exact order/effect
	CylinderMeshComponent->SetStaticMesh(CylinderMesh.Object);																//attach/set the mesh to the mesh object selected
	//CylinderMeshComponent->SetupAttachment(RootComponent);
	CylinderMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);							//add collision

	CylinderMeshComponent->SetSimulatePhysics(true);																		//set physics for collisions
	CylinderMeshComponent->SetLinearDamping(10000);																			//use big physics values so the player cannot exert a force on the object
	CylinderMeshComponent->SetAngularDamping(10000);																		// " "
	CylinderMeshComponent->SetEnableGravity(false);																			//no gravity
	CylinderMeshComponent->SetConstraintMode(EDOFMode::XYPlane);															//limit axis movement - no Z movement
	CylinderMeshComponent->SetMassOverrideInKg(NAME_None, 10000);															//big value - no impulses
	CylinderMeshComponent->bIgnoreRadialImpulse = true;																		//no impulses
	CylinderMeshComponent->bIgnoreRadialForce = true;																		// " "
	CylinderMeshComponent->bApplyImpulseOnDamage = false;																	// " "
	CylinderMeshComponent->bReplicatePhysicsToAutonomousProxy = false;														//this is default but wanted to move all here for clarity

	CylinderMeshComponent->SetNotifyRigidBodyCollision(true);																//ensure rigid body is set, more consistent collision + physics
	CylinderMeshComponent->OnComponentHit.AddDynamic(this, &ABasicSlugEnemy::OnHit);										//set up a notification for when this component hits something

	//keep track of the players location so we can move towards them
	playerLocation = FVector(0, 0, 0);

	//enemies start in the "off" state where they will not call their respective tick functions
	isEnemyActive = false;

	//default values for this specific enemy type
	moveSpeed = 100;
	slugHealth = 40;
	slugDamage = 15;
}


//Functions to control the basic functionality of the slug enemy object///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ABasicSlugEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetRoomPlacementModifier();																					//get the Z distance or vector between rooms
}
void ABasicSlugEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isEnemyActive == true)																					//check if enemy is on
	{
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)				//Get player actor
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			playerLocation = ActorItr->GetPlayersCurrentLocation();												//get players locations
		}
		MoveTowardsPlayer(DeltaTime);																			//call move towards player and pass current time
	}
}

//Checks if the slug enemy has been hit, this function should be present on all enemy objects////////////////////////////////////////////////////////////////////////////////////////////////////////
void ABasicSlugEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString actorName;																								//holds the name of the hit actor
	float playerDmg = 0;																							//default damage value - never pass empty value
	float moveSpeedModifier = 1;																					//speed modifier default - never pass empty value

	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		actorName = *OtherActor->GetName();																			//set actor name to actor hit

		if (actorName == "TP_TwinStickPawn_1")																		//check if it matches character name
		{
			for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
				ActorItr->PlayerTakeDamage(slugDamage);																//if yes player takes damage
			}
		}
		else if (actorName.Contains("DungeonsNDwellingsv4Projectile_"))												//check if it matches projectile name 
		{
			OtherActor->Destroy();																					//if yes destroy projectile

			for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
				playerDmg = ActorItr->GetProjectileDamage();														//take damage == to player damage
				moveSpeedModifier = ActorItr->GetMoveSpeedModifier();												//modify speed if a modifier is being applied else == 1
			}
			this->SetSlugMoveSpeed(moveSpeedModifier);																//call functions for specific enemy
			this->SlugTakeDamage(playerDmg);																		// " "
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control the behaviour of the slug enemies///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ABasicSlugEnemy::SetIsEnemyActive()								//activate enemy on call
{
	isEnemyActive = true;												//set active
	GetWorld()->GetTimerManager().SetTimer(moveSpeed_TimerHandle, this, &ABasicSlugEnemy::MaintainSlugMovementSpeed, 1.f, true, 0.f); //only active enemies need there move speed maintained 
}
void ABasicSlugEnemy::MoveTowardsPlayer(float deltaTime)							//move enemy towards player
{
	FVector myLocation = GetActorLocation();										//get current location for enemy
	FVector distanceToPlayer = myLocation - playerLocation;							//get distance to player
	distanceToPlayer.Normalize();													//normalise distance "dur"

	FVector velocity = (distanceToPlayer * moveSpeed * deltaTime) * (-1);			//calculate velocity then * -1 - probs could switch around previous equation but oh well

	velocity.Z = 0;																	//cancel any Z velocity

	SetActorLocation(myLocation + velocity);										//update location on tick
}
void ABasicSlugEnemy::MaintainSlugMovementSpeed()					//maintain slug movespeed
{
	moveSpeed = 100;												//after a slug is "Slowed" or similar by player this will restore the speed after a small interval
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control slug enemies taking damage and the application of status effects////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ABasicSlugEnemy::SlugTakeDamage(float dmg)																		//take damage function
{
	float zLoc = GetZLocation();																					//get the zLocation, id's which room an enemy is in
	FString enemyName = GetName();																					//gets the name of the object
	int roomNumber = FMath::FloorToInt(zLoc / roomPlacementModifier.Z);												//calculates the exact room number

	slugHealth -= dmg;																								//reduce slug health
	if (slugHealth <= 0)																							//check if slug should die
	{
		for (TActorIterator<AEnemySpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AEnemySpawner *Object = *ActorItr;
			ActorItr->CheckRoomCleared(roomNumber);																	//everytime slug dies check if room is clear
			ActorItr->RemoveArrayItem(enemyName);																	//remove from enemy array
		}
		Destroy();																									//destroy after removing from array
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to GET and SET key variables between classes//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float ABasicSlugEnemy::GetZLocation()																				//gets the Z co-ord, Z co-ords indicate room number
{
	FVector loc = GetActorLocation();												//location
	float zLoc = loc.Z;																//z value
	return (zLoc);
}
float ABasicSlugEnemy::GetZOffset()				//for passing how far off the ground an enemy is, not the full height
{
	return (slugZOffset);						//how far an object is positioned off the floor, stops collisions with the ground 			
}
void ABasicSlugEnemy::GetRoomPlacementModifier()  //how far apart are rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->GetRoomPlacementModifier(); //gets distance vector - should = 2000 on Z
	}
}
void ABasicSlugEnemy::SetSlugMoveSpeed(float speedModifier) //allows other objects to modify slug movespeed
{
	moveSpeed = moveSpeed * speedModifier; //speed = speed * % change
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
