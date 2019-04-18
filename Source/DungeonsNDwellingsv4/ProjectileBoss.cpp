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

	static ConstructorHelpers::FObjectFinder<UStaticMesh>ProjectileBossMesh(TEXT("/Game/TwinStick/Meshes/ProjectileBoss.ProjectileBoss"));

	//Set up collsion and appropriate physics for the boss object
	ProjectileBossComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BossMesh0"));
	RootComponent = ProjectileBossComponent;
	ProjectileBossComponent->SetStaticMesh(ProjectileBossMesh.Object);
	ProjectileBossComponent->SetupAttachment(RootComponent);
	ProjectileBossComponent->BodyInstance.SetCollisionProfileName("BlockAllDynamic");

	ProjectileBossComponent->SetSimulatePhysics(false);
	ProjectileBossComponent->SetLinearDamping(100000);
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
	gunOffset = FVector(50, 0, 0);

	//Set up boss stats
	projectileBossContactDamage = 25;
	projectileBossDmg = 20;
	projectileBossHealth = 200;

	//is boss turned on - off by default
	isBossActive = false;
}


//Functions to control the basic functionality of the Projectile Boss object//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AProjectileBoss::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString actorName;

	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		actorName = *OtherActor->GetName();

		if (actorName == "TP_TwinStickPawn_1")
		{
			for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
				ActorItr->takeDamage(projectileBossContactDamage);
			}
		}
		else if (actorName.Contains("DungeonsNDwellingsv4Projectile_"))
		{
			for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
				playerDmg = ActorItr->GetProjectileDamage();
			}
			this->BossTakeDamage(playerDmg);
		}
	}
}

// Called when the game starts or when spawned
void AProjectileBoss::BeginPlay()
{
	Super::BeginPlay();

	GetRoomCount();
	GetSelfZOffset();
	
	loc = GetActorLocation();
}

// Called every frame
void AProjectileBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isBossActive == true)
	{
		SetActorLocation(loc);
		SetActorRotation(rot);
		SetActorScale3D(sca);

		for (TActorIterator<ADungeonsNDwellingsv4Pawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADungeonsNDwellingsv4Pawn *Object = *ActorItr;
			playerLocation = ActorItr->getCurrentLocation();
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control boss behaviour///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AProjectileBoss::SetIsBossActive()
{
	isBossActive = true;
	SelectBossAttack();
}

void AProjectileBoss::SelectBossAttack()
{
	int attackSelector;
	FString attackName;
	float endTime;
	FTimerDelegate timerDel;
	FTimerDelegate timerDel_Two;
	float timeToRun;
		
	attackSelector = FMath::RandRange(0, (bossAttacks.Num()-1));
	attackName = bossAttacks[attackSelector];
	endTime = GetWorld()->GetTimeSeconds() + 6;

	if (attackName == "straight")
	{
		timeToRun = 1.8;
		endTime = GetWorld()->GetTimeSeconds() + timeToRun;
		//Binding the function with specific values
		timerDel.BindUFunction(this, FName("StraightAttack"), endTime);
		//Calling straight attack function after 1s and loop
		GetWorld()->GetTimerManager().SetTimer(attackTimerHandle, timerDel, 0.15f, true, 1.f);
	}
	else if (attackName == "circular")
	{
		timeToRun = 4.5;
		endTime = GetWorld()->GetTimeSeconds() + timeToRun;
		//Binding the function with specific values
		timerDel.BindUFunction(this, FName("CircularAttack"), endTime);
		//Calling straight attack function after 1s and loop
		GetWorld()->GetTimerManager().SetTimer(attackTimerHandle, timerDel, 0.5f, true, 1.f);
	}
	else if (attackName == "pulsating")
	{
		timeToRun = 8;
		endTime = GetWorld()->GetTimeSeconds() + timeToRun;
		//Binding the function with specific values
		timerDel.BindUFunction(this, FName("PulsatingAttack_One"), endTime);
		timerDel_Two.BindUFunction(this, FName("PulsatingAttack_Two"), endTime);
		//Calling straight attack function after 1s and loop
		GetWorld()->GetTimerManager().SetTimer(attackTimerHandle, timerDel, 0.7f, true, 0.5f);
		GetWorld()->GetTimerManager().SetTimer(attackTimerHandle_Two, timerDel_Two, 0.7f, true, 0.7f);
	}
}

void AProjectileBoss::StraightAttack(float endTime)
{
	float currentTime = GetWorld()->GetTimeSeconds();
	
	if (currentTime <= endTime)
	{
		FVector direction = playerLocation - GetActorLocation();
		const FRotator FireRotation = direction.Rotation();
		// Spawn projectile at an offset from this pawn
		const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(gunOffset);
		const FVector Scale = FVector(1, 1, 1);
		const FTransform spawnLocation = FTransform(FireRotation, SpawnLocation, Scale);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			AEnemyProjectile* projectileActor = World->SpawnActor<AEnemyProjectile>(AEnemyProjectile::StaticClass(), spawnLocation);
		}
	}
	else
	{
		//Ensure the timer is cleared by using the timer handle
		GetWorld()->GetTimerManager().ClearTimer(attackTimerHandle);
		SelectBossAttack();
	}
}

void AProjectileBoss::CircularAttack(float endTime)
{
	float currentTime = GetWorld()->GetTimeSeconds();

	FVector sca = FVector(1, 1, 1);
	float radius = 25;
	int quantity = 10;

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
				position = (360 / quantity) * i;
				rotation = FRotator(0, position, 0);
				forwardVector = UKismetMathLibrary::GetForwardVector(rotation);
				const FRotator FireRotation = forwardVector.Rotation();
				spawnVector = ((forwardVector * radius) + GetActorLocation()) + FireRotation.RotateVector(gunOffset);
				spawnLocation = FTransform(FireRotation, spawnVector, sca);
				AEnemyProjectile* projectileActor = World->SpawnActor<AEnemyProjectile>(AEnemyProjectile::StaticClass(), spawnLocation);
			}
		}
	}
	else
	{
		//Ensure the timer is cleared by using the timer handle
		GetWorld()->GetTimerManager().ClearTimer(attackTimerHandle);
		SelectBossAttack();
	}
}

void AProjectileBoss::PulsatingAttack_One(float endTime)
{
	float currentTime = GetWorld()->GetTimeSeconds();
	
	FVector sca = FVector(1, 1, 1);
	float radius = 35;
	int quantity = 10;

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
				position = (360 / quantity) * i;
				rotation = FRotator(0, position, 0);
				forwardVector = UKismetMathLibrary::GetForwardVector(rotation);
				const FRotator FireRotation = forwardVector.Rotation();
				spawnVector = ((forwardVector * radius) + GetActorLocation()) + FireRotation.RotateVector(gunOffset);
				spawnLocation = FTransform(FireRotation, spawnVector, sca);
				AEnemyProjectile* projectileActor = World->SpawnActor<AEnemyProjectile>(AEnemyProjectile::StaticClass(), spawnLocation);
			}
		}
	}
	else
	{
		//Ensure the timer is cleared by using the timer handle
		GetWorld()->GetTimerManager().ClearTimer(attackTimerHandle);
	}
}

void AProjectileBoss::PulsatingAttack_Two(float endTime)
{
	float currentTime = GetWorld()->GetTimeSeconds();
	
	FVector sca = FVector(1, 1, 1);
	float radius = 20;
	int quantity = 10;

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
				position = ((360 / quantity) * i) + 18;
				rotation = FRotator(0, position, 0);
				forwardVector = UKismetMathLibrary::GetForwardVector(rotation);
				const FRotator FireRotation = forwardVector.Rotation();
				spawnVector = ((forwardVector * radius) + GetActorLocation()) + FireRotation.RotateVector(gunOffset);
				spawnLocation = FTransform(FireRotation, spawnVector, sca);
				AEnemyProjectile* projectileActor = World->SpawnActorDeferred<AEnemyProjectile>(AEnemyProjectile::StaticClass(), spawnLocation);
				projectileActor->UpdateMaterials();
				projectileActor->FinishSpawning(spawnLocation);
			}
		}
	}
	else
	{
		//Ensure the timer is cleared by using the timer handle
		GetWorld()->GetTimerManager().ClearTimer(attackTimerHandle_Two);
		SelectBossAttack();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control boss taking damage and negative effects//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AProjectileBoss::BossTakeDamage(float dmg)
{
	projectileBossHealth -= dmg;

	if (projectileBossHealth <= 0)
	{
		for (TActorIterator<ADoorSealSpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ADoorSealSpawner *Object = *ActorItr;
			ActorItr->openDoors(roomCount);
		}
		for (TActorIterator<ABossManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			ABossManager *Object = *ActorItr;
			ActorItr->SetIsBossKilled();
		}
		Destroy();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to GET and SET key variables from external functions////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AProjectileBoss::GetSelfZOffset()
{
	for (TActorIterator<ABossManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ABossManager *Object = *ActorItr;
		zOffset = ActorItr->GetZOffset();
	}
}

void AProjectileBoss::GetRoomCount()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->getRoomCount();
	}
}
float AProjectileBoss::GetBossProjectileDmg()
{
	return projectileBossDmg;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
