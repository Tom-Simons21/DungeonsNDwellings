// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "DungeonsNDwellingsv4Pawn.h"
#include "DungeonsNDwellingsv4Projectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Engine.h"
#include "InteractableObject.h"
#include "TileGeneratorParent.h"
#include "DoorSeal.h"
#include "EnemySpawner.h"
#include "BossManager.h"

const FName ADungeonsNDwellingsv4Pawn::MoveForwardBinding("MoveForward");
const FName ADungeonsNDwellingsv4Pawn::MoveRightBinding("MoveRight");
const FName ADungeonsNDwellingsv4Pawn::FireForwardBinding("FireForward");
const FName ADungeonsNDwellingsv4Pawn::FireRightBinding("FireRight");
static float rateOfFire = 0.3f;

ADungeonsNDwellingsv4Pawn::ADungeonsNDwellingsv4Pawn()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/Cube.Cube"));
	// Create the mesh component
	PlayerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = PlayerMeshComponent;
	PlayerMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	PlayerMeshComponent->SetStaticMesh(ShipMesh.Object);

	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when ship does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->RelativeRotation = FRotator(-90.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm
	CameraComponent->FieldOfView = 66;
	CameraComponent->AddRelativeLocation(FVector(0, 0, 0));
	CameraComponent->SetAspectRatio(1.0);

	// Movement
	MoveSpeed = 1000.0f;
	playerZElevation = FVector(0, 0, 22);
	
	// Weapon
	GunOffset = FVector(36.f, 0.f, 0.f);
	bCanFire = true;

	//set values to be used for projectile
	initialSpeed = 250;
	maxSpeed = 250;
	lifeSpan = 2;
	projectileDamage = 10;

	//room modifier values
	roomPlacementModifier = FVector(0, 0, 2000);

	doorStartPoints.Empty();
	doorEndPoints.Empty();


	playerHealth = 100;
}

void ADungeonsNDwellingsv4Pawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADungeonsNDwellingsv4Pawn::OnInteract);
}

void ADungeonsNDwellingsv4Pawn::Tick(float DeltaSeconds)
{
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		const FRotator NewRotation = Movement.Rotation();
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);

		if (Hit.IsValidBlockingHit())
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
			RootComponent->MoveComponent(Deflection, NewRotation, true);
		}
	}

	// Create fire direction vector
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);
	const float FireRightValue = GetInputAxisValue(FireRightBinding);
	const FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);

	// Try and fire a shot
	FireShot(FireDirection);

	getPlayerLocation();
}

void ADungeonsNDwellingsv4Pawn::FireShot(FVector FireDirection)
{
	// If it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);
			const FVector Scale = FVector(1, 1, 1);

			const FTransform SpawnPosition = FTransform(FireRotation, SpawnLocation, Scale);

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile
				ADungeonsNDwellingsv4Projectile* projectileActor = World->SpawnActorDeferred<ADungeonsNDwellingsv4Projectile>(ADungeonsNDwellingsv4Projectile::StaticClass(), SpawnPosition);
				projectileActor->updateProperties(initialSpeed, maxSpeed, lifeSpan);
				projectileActor->FinishSpawning(SpawnPosition);
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ADungeonsNDwellingsv4Pawn::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ShotTimerExpired()
{
	bCanFire = true;
}

void ADungeonsNDwellingsv4Pawn::BeginPlay()
{
	Super::BeginPlay();

	isDamageable = false;

	FTimerHandle hitControlTimer;

	GetWorldTimerManager().SetTimer(hitControlTimer, this, &ADungeonsNDwellingsv4Pawn::makeDamageable, 1.5f, true, 2.0f);

	FireRate = updateProperties(rateOfFire);

	FVector ActorLocation = (FVector(175, 400, 0) + playerZElevation);
	SetActorLocation(ActorLocation, false);
	SetActorScale3D(FVector(0.4, 0.4, 0.4));

	createArrayOfDoors();
}

void ADungeonsNDwellingsv4Pawn::OnInteract()
{
	for (TActorIterator<AInteractableObject> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AInteractableObject *Object = *ActorItr;
		ActorItr->playerTakesItem();
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("I'm Pressing Action"));
	}
}




void ADungeonsNDwellingsv4Pawn::getPlayerLocation()
{
	FVector actorLoc = GetActorLocation();
	FVector actorZVector = FVector(0.f, 0.f, 0.f);

	actorZVector.Z += actorLoc.Z;

	for (TActorIterator<AInteractableObject> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AInteractableObject *Object = *ActorItr;
		ActorItr->getPlayerLocation(actorLoc);
	}

	checkPlayerLocation(actorLoc, actorZVector);
}

FVector ADungeonsNDwellingsv4Pawn::getCurrentLocation()
{
	FVector currentLoc = GetActorLocation();

	return (currentLoc);
}




void ADungeonsNDwellingsv4Pawn::updateProjectileValues(float initSpeed, float topSpeed, float lifeTime)
{
	initialSpeed = initSpeed;
	maxSpeed = topSpeed;
	lifeSpan = lifeTime;
}

float ADungeonsNDwellingsv4Pawn::updateProperties(float defaultValue)
{
	float rateOfFire = defaultValue;

	return(rateOfFire);
}




void ADungeonsNDwellingsv4Pawn::checkPlayerLocation(FVector playerCurrentLoc, FVector actorZValue)
{
	FVector playerPosition = playerCurrentLoc;
	FVector playerZPosition = actorZValue;
	FVector newLocationAdjust;
	FVector doorLocation;
	int doorNum;

	if ((playerPosition.X <= 0 || playerPosition.X >= 800) && (playerPosition.Y <= 430 && playerPosition.Y >= 370))
	{
		if (playerPosition.X <= 0)
		{
			newLocationAdjust = FVector(100, 400, 0);
			doorLocation = FVector(0, 400, 0);
			doorNum = 1;
		}
		else if (playerPosition.X >= 800)
		{
			newLocationAdjust = FVector(700, 400, 0);
			doorLocation = FVector(0, 400, 0);
			doorNum = 3;
		}
		moveToRoom(playerZPosition, newLocationAdjust);
	}
	else if ((playerPosition.Y <= 0 || playerPosition.Y >= 800) && (playerPosition.X <= 430 && playerPosition.X >= 370))
	{
		if (playerPosition.Y <= 0)
		{
			newLocationAdjust = FVector(400, 100, 0);
			doorLocation = FVector(400, 0, 0);
			doorNum = 4;
		}
		else if (playerPosition.Y >= 800)
		{
			newLocationAdjust = FVector(400, 700, 0);
			doorLocation = FVector(400, 800, 0);
			doorNum = 3;
		}
		moveToRoom(playerZPosition, newLocationAdjust);
	}
}

void ADungeonsNDwellingsv4Pawn::moveToRoom(FVector actorZ, FVector doorLocation)
{
	FVector playerZ = actorZ;   //this equals 22 or will equal 22 + (roomPlacementModifier * floorNumber)
	FVector playerZNoElevation = (playerZ - playerZElevation);
	int floorPickMultiplier;
	FVector floorCoord;
	bool isNewRoom;
	FVector playerNewLoc;

	do
	{
		isNewRoom = true;
		floorPickMultiplier = FMath::RandRange(0, 5);
		floorCoord = floorPickMultiplier * roomPlacementModifier;

		if (floorCoord.Z == playerZNoElevation.Z)
		{
			isNewRoom = false;
		}

	} while (isNewRoom == false);

	playerNewLoc = doorLocation + floorCoord + playerZElevation;

	for (TActorIterator<AEnemySpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AEnemySpawner *Object = *ActorItr;
		ActorItr->activateEnemies(playerNewLoc);
	}
	for (TActorIterator<ABossManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ABossManager *Object = *ActorItr;
		ActorItr->ActivateBoss(playerNewLoc);
	}

	SetActorLocation(playerNewLoc, false);
}







void ADungeonsNDwellingsv4Pawn::createArrayOfDoors()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		arrayOfDoors = ActorItr->getArrayOfDoors();
	}
}

void ADungeonsNDwellingsv4Pawn::getTotalOfDoors()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		totalDoorNum = ActorItr->getRunningTotal();
	}
}







/*
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player Z Location is: %s"), *playerCoord));
	}
	*/



void ADungeonsNDwellingsv4Pawn::makeDamageable()
{
	bool makeDamageable = true;

	setIsDamageable(makeDamageable);
}

void ADungeonsNDwellingsv4Pawn::setIsDamageable(bool isD)
{
	isDamageable = isD;
}

void ADungeonsNDwellingsv4Pawn::takeDamage(float dmg)
{
	if (isDamageable == true)
	{
		playerHealth -= dmg;

		isDamageable = false;
	}
}