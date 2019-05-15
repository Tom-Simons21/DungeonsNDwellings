// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileBoss.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "DungeonsNDwellingsv4Projectile.h"
#include "DoorSealSpawner.h"
#include "BossManager.h"
#include "TileGeneratorParent.h"
#include "EnemyProjectile.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"

// Sets default values
AProjectileBoss::AProjectileBoss()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>ProjectileBossMesh(TEXT("/Game/TwinStick/Meshes/ProjectileBoss.ProjectileBoss")); //select mesh object

	//Set up collsion and appropriate physics for the boss object
	ProjectileBossComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BossMesh0"));	//create mesh
	RootComponent = ProjectileBossComponent;													//create root
	ProjectileBossComponent->SetStaticMesh(ProjectileBossMesh.Object);							//attach object/geometry to mesh object
	//ProjectileBossComponent->SetupAttachment(RootComponent);
	ProjectileBossComponent->BodyInstance.SetCollisionProfileName("BlockAllDynamic");			//set collision

	ProjectileBossComponent->SetSimulatePhysics(false);											//not simulating physics cos I'm and OCD overly obsessive idiot who wanted his boss to be roundish
	ProjectileBossComponent->SetLinearDamping(100000);											//big number again, see comments for slug
	ProjectileBossComponent->SetAngularDamping(100000);
	ProjectileBossComponent->SetEnableGravity(false);
	ProjectileBossComponent->SetConstraintMode(EDOFMode::XYPlane);
	ProjectileBossComponent->SetMassOverrideInKg(NAME_None, 1000000);
	ProjectileBossComponent->bIgnoreRadialImpulse = true;
	ProjectileBossComponent->bIgnoreRadialForce = true;
	ProjectileBossComponent->bApplyImpulseOnDamage = false;
	ProjectileBossComponent->bReplicatePhysicsToAutonomousProxy = false;

	ProjectileBossComponent->SetNotifyRigidBodyCollision(true);
	ProjectileBossComponent->OnComponentHit.AddDynamic(this, &AProjectileBoss::OnHit);	//set up a notification for when this component hits something

	//set up weapon variance
	gunOffset = FVector(70, 0, 0);					//offset from the mesh centre for projectiles to spawn

	//Set up boss stats
	projectileBossContactDamage = 25;				//damage for if player touches boss
	projectileBossDmg = 20;							//damage boss projectiles do
	projectileBossHealth = 300;						//boss health

	//is boss turned on - off by default
	isBossActive = false;
}

//Functions to control collision and hit detection on projectile boss//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AProjectileBoss::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString actorName;																//name of the actor hit
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		actorName = *OtherActor->GetName();				//set to actor hit
		if (actorName == "TP_TwinStickPawn_1")			//check if its the player
		{
			for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
				ActorItr->PlayerTakeDamage(projectileBossContactDamage);		//if hits player then player takes damages
			}
		}
		else if (actorName.Contains("DungeonsNDwellingsv4Projectile_"))			//check if its the projectile
		{
			for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
				playerDmg = ActorItr->GetProjectileDamage();					//player deals damage to boss
			}
			this->BossTakeDamage(playerDmg);									//deal the damage
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control basic functionality/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AProjectileBoss::BeginPlay()
{
	Super::BeginPlay();

	GetRoomCount();								//get number of rooms
	loc = GetActorLocation();					//get bosses location
}
void AProjectileBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isBossActive == true)				//if boss is on
	{
		SetActorLocation(loc);				//maintain constant location, boss shouldn't move
		SetActorRotation(rot);				//maintain
		SetActorScale3D(sca);				//maintain
		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			playerLocation = ActorItr->GetPlayersCurrentLocation();	//constantly check player location
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control boss attack decision making//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AProjectileBoss::SetIsBossActive()	//activate boss/turn on
{
	isBossActive = true;		//turn on
	SelectBossAttack();			//once boss is on it starts attacking
}
void AProjectileBoss::SelectBossAttack()
{
	int attackSelector;					//value to hold the attack picked by random generator
	FString attackName;					//name of the attack selected
	float endTime;						//the time when the current attack should end
	FTimerDelegate timerDel;			//timer for attacks 1, 2, 3.0
	FTimerDelegate timerDel_Two;		//timer for attack 3.1
	float timeToRun;					//the amount of time to add to the end for each attack
		
	attackSelector = FMath::RandRange(0, (bossAttacks.Num()-1));	//select a random attack from array list of attacks
	attackName = bossAttacks[attackSelector];						//get the string value from array

	if (attackName == "straight")			//check which attack picked
	{
		timeToRun = 1.8;	//time for this attack to run for
		endTime = GetWorld()->GetTimeSeconds() + timeToRun;	//run for = current time + run time
		//Binding the function with specific values
		timerDel.BindUFunction(this, FName("StraightAttack"), endTime);
		//Calling straight attack function after 1s and loop
		GetWorld()->GetTimerManager().SetTimer(attackTimerHandle, timerDel, 0.15f, true, 1.f);
	}
	else if (attackName == "circular") //comments repeat for each attack
	{
		timeToRun = 3.0;
		endTime = GetWorld()->GetTimeSeconds() + timeToRun;
		//Binding the function with specific values
		timerDel.BindUFunction(this, FName("CircularAttack"), endTime);
		//Calling circular attack function after 1s and loop
		GetWorld()->GetTimerManager().SetTimer(attackTimerHandle, timerDel, 0.5f, true, 1.f);
	}
	else if (attackName == "pulsating")	//has two timers so it can spawn 2 objects on the correct sequence, simpler than trying to constantly modify the spawning
	{
		timeToRun = 4.2;
		endTime = GetWorld()->GetTimeSeconds() + timeToRun;
		//Binding the function with specific values
		timerDel.BindUFunction(this, FName("PulsatingAttack_One"), endTime);
		timerDel_Two.BindUFunction(this, FName("PulsatingAttack_Two"), endTime);
		//Calling pulsating attack x2 function after .5s / .7s respectively and loop
		GetWorld()->GetTimerManager().SetTimer(attackTimerHandle, timerDel, 0.7f, true, 0.5f);
		GetWorld()->GetTimerManager().SetTimer(attackTimerHandle_Two, timerDel_Two, 0.7f, true, 0.7f);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to perform all boss attacks////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AProjectileBoss::StraightAttack(float endTime)
{
	float currentTime = GetWorld()->GetTimeSeconds();	//compare the current time to calculated end time
	
	if (currentTime <= endTime)
	{
		FVector direction = playerLocation - GetActorLocation();	//aim at player
		const FRotator FireRotation = direction.Rotation();			//aim at player
		// Spawn projectile at an offset from this pawn
		const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(gunOffset);	//spawn projectile aimed at player, at the preset offset from actor
		const FVector Scale = FVector(1, 1, 1);	//set scale
		const FTransform spawnLocation = FTransform(FireRotation, SpawnLocation, Scale);	//setup transform

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			AEnemyProjectile* projectileActor = World->SpawnActor<AEnemyProjectile>(AEnemyProjectile::StaticClass(), spawnLocation);	//spawn
		}
	}
	else
	{
		//Ensure the timer is cleared by using the timer handle
		GetWorld()->GetTimerManager().ClearTimer(attackTimerHandle);	//once time span has completed clear the timer so it can be started again when needed
		SelectBossAttack();		//select a new attack
	}
}
void AProjectileBoss::CircularAttack(float endTime)
{
	float currentTime = GetWorld()->GetTimeSeconds();

	FVector sca = FVector(1, 1, 1);		//set scale
	float radius = 25;					//radius at which to spawn
	int quantity = 14;					//number of projectiles to spawn

	float position;						//holds the X rotation for the spawning location
	FRotator rotation;					//rotation at which to spawn projectile
	FVector forwardVector;				//forward speed values
	FVector spawnVector;				//spawn position
	FTransform spawnLocation;			//setup transform for spawning

	if (currentTime <= endTime)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			for (int i = 1; i <= quantity; i++)			//spawn projectiles = to quantity
			{
				position = (360 / quantity) * i;				//position is the degree of rotation around a circle
				rotation = FRotator(0, position, 0);			//rotate around X axis
				forwardVector = UKismetMathLibrary::GetForwardVector(rotation);	//forward speed = forward vector made from the rotation, moves at that angle constantly
				const FRotator FireRotation = forwardVector.Rotation();			//fire rotation uses forward vector rotation
				spawnVector = ((forwardVector * radius) + GetActorLocation()) + FireRotation.RotateVector(gunOffset - FVector(15, 0, 0));	//set up position to spawn based on circular position / rotation + gun offset
				spawnLocation = FTransform(FireRotation, spawnVector, sca);		//setup transform
				AEnemyProjectile* projectileActor = World->SpawnActor<AEnemyProjectile>(AEnemyProjectile::StaticClass(), spawnLocation); //spawn
			}
		}
	}
	else
	{
		//Ensure the timer is cleared by using the timer handle
		GetWorld()->GetTimerManager().ClearTimer(attackTimerHandle); //clear timer on complete
		SelectBossAttack();		//call next attack
	}
}
void AProjectileBoss::PulsatingAttack_One(float endTime)
{
	float currentTime = GetWorld()->GetTimeSeconds();
	
	FVector sca = FVector(1, 1, 1); //scale
	float radius = 35;	//distance from actor to spawn
	int quantity = 9;	//number of projectiles to spawn

	float position;						//uses the exact same system as the circular
	FRotator rotation;
	FVector forwardVector;
	FVector spawnVector;
	FTransform spawnLocation;
	
	if (currentTime <= endTime)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			for (int i = 1; i <= quantity; i++)
			{
				position = (360 / quantity) * i;
				rotation = FRotator(0, position, 0);
				forwardVector = UKismetMathLibrary::GetForwardVector(rotation);
				const FRotator FireRotation = forwardVector.Rotation();
				spawnVector = ((forwardVector * radius) + GetActorLocation()) + FireRotation.RotateVector(gunOffset - FVector(15, 0, 0));
				spawnLocation = FTransform(FireRotation, spawnVector, sca);
				AEnemyProjectile* projectileActor = World->SpawnActor<AEnemyProjectile>(AEnemyProjectile::StaticClass(), spawnLocation);
			}
		}
	}
	else
	{
		//Ensure the timer is cleared by using the timer handle
		GetWorld()->GetTimerManager().ClearTimer(attackTimerHandle); //clear timer - next attack is called from the second part of the attack
	}
}
void AProjectileBoss::PulsatingAttack_Two(float endTime)
{
	float currentTime = GetWorld()->GetTimeSeconds();
	
	FVector sca = FVector(1, 1, 1);
	float radius = 20;					//uses different radius
	int quantity = 9;					//same quantity

	float position;
	FRotator rotation;
	FVector forwardVector;
	FVector spawnVector;
	FTransform spawnLocation;
	
	if (currentTime <= endTime)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			for (int i = 1; i <= quantity; i++)
			{
				position = ((360 / quantity) * i) + 20;	//add 18 this provides an additional 50% rotation
				rotation = FRotator(0, position, 0);
				forwardVector = UKismetMathLibrary::GetForwardVector(rotation);
				const FRotator FireRotation = forwardVector.Rotation();
				spawnVector = ((forwardVector * radius) + GetActorLocation()) + FireRotation.RotateVector(gunOffset - FVector(15, 0, 0));
				spawnLocation = FTransform(FireRotation, spawnVector, sca);
				AEnemyProjectile* projectileActor = World->SpawnActorDeferred<AEnemyProjectile>(AEnemyProjectile::StaticClass(), spawnLocation); //change material so players can more easily distinguish
				projectileActor->UpdateMaterials();		//change materials
				projectileActor->FinishSpawning(spawnLocation);
			}
		}
	}
	else
	{
		//Ensure the timer is cleared by using the timer handle
		GetWorld()->GetTimerManager().ClearTimer(attackTimerHandle_Two); //clear second timer
		SelectBossAttack(); //select next attack
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to control boss taking damage and negative effects/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AProjectileBoss::BossTakeDamage(float dmg)
{
	projectileBossHealth -= dmg;		//take damage
		
	if (projectileBossHealth <= 0)		//check if boss should be dead
	{
		for (TActorIterator<ADoorSealSpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADoorSealSpawner *Object = *ActorItr;
			ActorItr->OpenDoors(roomCount);				//if boss should be dead open room doors
		}
		for (TActorIterator<ABossManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ABossManager *Object = *ActorItr;
			ActorItr->SetIsBossKilled();				//set boss is dead
		}
		Destroy();			//destroy
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Functions to GET and SET key variables from external functions///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AProjectileBoss::GetRoomCount()			//get number of rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->GetRoomCount();
	}
}
float AProjectileBoss::GetBossProjectileDmg()		//pass damage to be dealt by attacks
{
	return projectileBossDmg;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
