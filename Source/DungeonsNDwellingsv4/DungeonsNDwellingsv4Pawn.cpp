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
#include "InteractableObjectManager.h"
#include "TileGeneratorParent.h"
#include "DoorSeal.h"
#include "EnemySpawner.h"
#include "BossManager.h"
#include "MyPlayerController.h"
#include "ItemManager.h"
#include "DoorPathingManager.h"
#include "DungeonsNDwellingsInstance.h"

const FName ADungeonsNDwellingsv4Pawn::MoveForwardBinding("MoveForward");
const FName ADungeonsNDwellingsv4Pawn::MoveRightBinding("MoveRight");
const FName ADungeonsNDwellingsv4Pawn::FireForwardBinding("FireForward");
const FName ADungeonsNDwellingsv4Pawn::FireRightBinding("FireRight");

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
	FireRate = 0.3;
	projectileScale = FVector(1, 1, 1);
	isGrowing = false;

	//projectile damage values
	projectileDefaultDamage = 10;
	projectileDamage = 10;

	//set values for player stats
	playerMaxHealth = 100;
	playerHealthDefault = 100;
	playerGold = 0;
	
	//Set currency modifier values 
	goldToAdd = 0;
	winStreak = 0;
	loseStreak = 0;

	//pause variables
	isPaused = false;

	//Set modifier values and stat changes variables
	strBuffActive = false;
	massBuffActive = false;
	vigBuffActive = false;
	sacBuffActive = false;

	spawnChanceValue = 0;
	healthRegenValue = 0;

	//maintain correct dmg stats
	otherDmgChanges = 0;

	//set enemy speed modifier
	enemySpeedReductionPercent = 1;
}

//Functions to control core functionality/////////////////////////////////////////////////////////////////////////////////////////////////////

void ADungeonsNDwellingsv4Pawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADungeonsNDwellingsv4Pawn::OnInteract);
	PlayerInputComponent->BindAction("Reroll", IE_Pressed, this, &ADungeonsNDwellingsv4Pawn::OnReroll);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &ADungeonsNDwellingsv4Pawn::OnPauseGame);
	PlayerInputComponent->BindAction("NextLevel", IE_Pressed, this, &ADungeonsNDwellingsv4Pawn::OnNextLevel);
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
	FVector spawnLocation;
	isSpawningSecondShot = false;

	// If it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			if (massBuffActive == true)
			{
				isSpawningSecondShot = SpawnAdditionalShots(FireDirection);
			}

			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			if (isSpawningSecondShot == true)
			{
				spawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset - FVector(0, 15, 0));
			}
			else
			{
				spawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);
			}
			const FVector Scale = FVector(1, 1, 1);
			const FTransform SpawnPosition = FTransform(FireRotation, spawnLocation, Scale);

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile
				ADungeonsNDwellingsv4Projectile* projectileActor = World->SpawnActorDeferred<ADungeonsNDwellingsv4Projectile>(ADungeonsNDwellingsv4Projectile::StaticClass(), SpawnPosition);
				projectileActor->updateProperties(initialSpeed, maxSpeed, lifeSpan, isGrowing, projectileScale);
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
	GetLevelNumber();

	isLevelComplete = false;

	playerHealth = playerHealthDefault;

	GetPlayerStatsFromGI();

	//Initialise all variables from external classes asap//////////////////////////////////////////////
	GetRoomPlacementModifier();
	getTotalOfDoors();
	createArrayOfDoors();
	GetRoomCount();
	GetDoorMappings();
	///////////////////////////////////////////////////////////////////////////////////////////////////

	isDamageable = false;
	GetWorldTimerManager().SetTimer(hitControlTimer, this, &ADungeonsNDwellingsv4Pawn::makeDamageable, 1.5f, true, 2.0f);

	SetActorLocation(playerStartPoint, false);
	SetActorScale3D(playerScale);
}

void ADungeonsNDwellingsv4Pawn::OnInteract()
{
	for (TActorIterator<AInteractableObjectManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AInteractableObjectManager *Object = *ActorItr;
		ActorItr->Claim();
	}
}

void ADungeonsNDwellingsv4Pawn::OnReroll()
{
	bool isRerolled = false;

	if (playerGold >= 2)
	{
		for (TActorIterator<AInteractableObjectManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AInteractableObjectManager *Object = *ActorItr;
			isRerolled = ActorItr->Reroll();
		}

		if (isRerolled == true)
		{
			playerGold -= 2;
		}
	}
}

void ADungeonsNDwellingsv4Pawn::OnPauseGame()
{
	UWorld* const World = GetWorld();
	UGameplayStatics::SetGamePaused(World, true);

	AMyPlayerController* const MyPlayer = Cast<AMyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (MyPlayer != NULL)
	{
		MyPlayer->OpenPauseMenu();
	}
}

void ADungeonsNDwellingsv4Pawn::OnNextLevel()
{
	FString nextLevel;
	FString nextLevelName;
	FName name;

	if (levelNumber + 1 < 6)
	{
		nextLevel = FString::FromInt(levelNumber + 1);
		nextLevelName = levelPrefix + nextLevel;

		name = FName(*nextLevelName);
	}
	else
	{
		name = "WinGame";
	}

	if (isLevelComplete)
	{
		for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AItemManager *Object = *ActorItr;
			ActorItr->TransitionToNewLevel();
		}

		SetGameInstanceVariables();
		UGameplayStatics::OpenLevel(GetWorld(), name);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control player currency and externals///////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Pawn::UpdatePlayerCurrency()
{
	int goldBonus;
	int interest;

	interest = playerGold / 10;

	if (interest > 5)
	{
		interest = 5;
	}
	if (playerHealth < 100)
	{
		winStreak = 0;
	}
	if (playerHealth >= 25)
	{
		loseStreak = 0;
	}

	if (winStreak > 1)
	{
		winStreak = 1;
	}
	else if (loseStreak > 1)
	{
		loseStreak = 1;
	}
	
	goldBonus = goldToAdd + interest + winStreak + loseStreak;

	playerGold += goldBonus;

	goldToAdd = 1;

	if (playerHealth == 100)
	{
		winStreak++;
	}
	else if (playerHealth < 25)
	{
		loseStreak++;
	}	

	if (playerGold > 100)
	{
		playerGold = 100;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions for item management and buff/debuff application////////////////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Pawn::SetStrBuff()
{
	strBuffActive = true;
}
void ADungeonsNDwellingsv4Pawn::SetMassBuff()
{
	massBuffActive = true;
}
void ADungeonsNDwellingsv4Pawn::SetVigBuff()
{
	vigBuffActive = true;
}
void ADungeonsNDwellingsv4Pawn::SetSacBuff()
{
	sacBuffActive = true;
}
void ADungeonsNDwellingsv4Pawn::SetRateBuff()
{
	rateBuffActive = true;
}
void ADungeonsNDwellingsv4Pawn::SetGrowthBuff()
{
	growthBuffActive = true;
}
void ADungeonsNDwellingsv4Pawn::SetSlowBuff()
{
	slowBuffActive = true;
}
void ADungeonsNDwellingsv4Pawn::SetHyperBuff()
{
	hyperBuffActive = true;
}
void ADungeonsNDwellingsv4Pawn::SetMnyShotBuff()
{
	mnyShotBuffActive = true;
}

void ADungeonsNDwellingsv4Pawn::ModifyPlayerDamage(bool isABuff, float damageMultiplier)
{
	bool strBuffMaxed = false;

	for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AItemManager *Object = *ActorItr;
		strBuffMaxed = ActorItr->IsStrBuffMaxed();
	}
	if (isABuff == true)
	{
		if (strBuffMaxed == true)
		{
			projectileDamage = (projectileDamage + otherDmgChanges) * damageMultiplier;
		}
		else if (strBuffActive == true)
		{
			projectileDamage = projectileDamage * damageMultiplier;
		}
	}
	if (isABuff == false)
	{
		if (strBuffMaxed != true)
		{
			projectileDamage = projectileDamage * damageMultiplier;
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyProjectileSpawnChance(bool isABuff, int spawnChanceModifier)
{
	if (massBuffActive == true)
	{
		spawnChanceValue = spawnChanceModifier;
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyPlayerHealth(bool isABuff, float healthIncrease, bool isHealthRegening, float healthRegenAmount)
{
	bool vigBuffMaxed = false;
	float currentHealthLost;

	for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AItemManager *Object = *ActorItr;
		vigBuffMaxed = ActorItr->IsVigBuffMaxed();
	}

	currentHealthLost = playerMaxHealth - playerHealth;
	if (currentHealthLost < 0)
	{
		currentHealthLost = 0;
	}

	if (isABuff == true)
	{
		if (vigBuffActive == true)
		{
			playerMaxHealth = playerHealthDefault + healthIncrease;
			playerHealth = playerHealthDefault + healthIncrease - currentHealthLost;

			if (isHealthRegening == true)
			{
				healthRegenValue = healthRegenAmount;
			}
		}
	}
	if (isABuff == false)
	{
		if (vigBuffMaxed != true)
		{
			playerHealth = playerHealth * healthIncrease;
			playerMaxHealth = playerHealth;
		}
		if (isHealthRegening == true)
		{
			healthRegenValue = (playerHealth / 10) * (-1);
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyPlayerKillBonuses(bool isABuff, float healthToRecieve, int chanceToRecieve)
{
	if (isABuff == true)
	{
		if (sacBuffActive == true)
		{
			healthFromKills = healthToRecieve;
			chanceToRecieveHealth = chanceToRecieve;
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyPlayerFireRate(bool isABuff, float fireRate)
{
	if (isABuff == true)
	{
		if (rateBuffActive == true)
		{
			FireRate = fireRate;
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyPlayerProjectileStyle(bool isABuff, float initSpeed, float topSpeed, float lifeTime, bool isGrow, FVector scale)
{
	if (isABuff == true)
	{
		if (growthBuffActive == true)
		{
			initialSpeed = initSpeed;
			maxSpeed = topSpeed;
			lifeSpan = lifeTime;
			isGrowing = isGrow;
			projectileScale = scale;
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyEnemyMoveSpeed(bool isABuff, float reductionPercent)
{
	if (isABuff == true)
	{
		if (slowBuffActive == true)
		{
			enemySpeedReductionPercent = reductionPercent;
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyMoneyDropChance(bool isABuff, float dropChanceIncrease)
{
	if (isABuff == true)
	{
		if (mnyShotBuffActive == true)
		{
			moneyDropModifier = dropChanceIncrease;

			for (TActorIterator<AEnemySpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				AEnemySpawner *Object = *ActorItr;
				ActorItr->SetMoneyDropChance(moneyDropModifier);
			}
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ActivateHyperMode(float percentDmgIncrease)
{
	float healthLost;
	float highHealthPenalty;
	float dmgIncrease = 0;

	healthLost = playerMaxHealth - playerHealth;
	if (healthLost < 0)
	{
		healthLost = 0;
	}
	else if (healthLost > (playerMaxHealth / 2))
	{
		dmgIncrease = healthLost * percentDmgIncrease;
	}
	else if (healthLost < (playerMaxHealth / 2))
	{
		highHealthPenalty = ((playerMaxHealth / 2) - healthLost) + 10;
		dmgIncrease = (highHealthPenalty / 20) * (-1);
	}

	if (hyperBuffActive == true)
	{
		projectileDamage += dmgIncrease;
		otherDmgChanges += dmgIncrease;
	}
}

bool ADungeonsNDwellingsv4Pawn::SpawnAdditionalShots(FVector FireDirection)
{
	bool secondShotSpawned = false;

	const FRotator FireRotation = FireDirection.Rotation();
	// Spawn projectile at an offset from this pawn
	const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset + FVector(0, 15, 0));
	const FVector Scale = FVector(1, 1, 1);

	const FTransform SpawnPosition = FTransform(FireRotation, SpawnLocation, Scale);

	int spawnChance = FMath::RandRange(1, spawnChanceValue);

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		if (spawnChance == 1)
		{
			secondShotSpawned = true;
			// spawn the projectile
			ADungeonsNDwellingsv4Projectile* projectileActor = World->SpawnActorDeferred<ADungeonsNDwellingsv4Projectile>(ADungeonsNDwellingsv4Projectile::StaticClass(), SpawnPosition);
			projectileActor->updateProperties(initialSpeed, maxSpeed, lifeSpan, isGrowing, projectileScale);
			projectileActor->FinishSpawning(SpawnPosition);
		}
	}

	return (secondShotSpawned);
}

void ADungeonsNDwellingsv4Pawn::RegenHealth()
{
	if (playerHealth < playerMaxHealth)
	{
		if (vigBuffActive == true)
		{
			playerHealth += healthRegenValue;
		}
	}

	if (playerHealth > playerMaxHealth)
	{
		playerHealth = playerMaxHealth;
	}
}

void ADungeonsNDwellingsv4Pawn::GainHealthOnKill()
{
	int chanceOfHealth;
	chanceOfHealth = FMath::RandRange(1, chanceToRecieveHealth);
	if (chanceOfHealth == 1)
	{
		playerHealth += healthFromKills;
	}
}

//Functions for modifying properties of the player projectile object///////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Pawn::UpdateProjectileValues(float initSpeed, float topSpeed, float lifeTime, bool growing, FVector scale)
{
	initialSpeed = initSpeed;
	maxSpeed = topSpeed;
	lifeSpan = lifeTime;
	isGrowing = growing;
	projectileScale = scale;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control and track player location and movement within a level///////////////////////////////////////////////////////////////////////////////////
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

void ADungeonsNDwellingsv4Pawn::checkPlayerLocation(FVector playerCurrentLoc, FVector actorZValue)
{
	FVector playerPosition = playerCurrentLoc;
	FVector playerZPosition = actorZValue;
	FVector newLocationAdjust;
	FVector doorLocation;

	if ((playerPosition.X <= 0 || playerPosition.X >= 800) && (playerPosition.Y <= 430 && playerPosition.Y >= 370))
	{
		if (playerPosition.X <= 0)
		{
			doorLocation = FVector(0, 400, 0);
			doorLocation.Z += playerCurrentLoc.Z - playerZElevation.Z;

			for (int i = 0; i < doorMapping.Num(); i++)
			{
				if (doorLocation == doorMapping[i])
				{
					if ((i % 2) == 0)
					{
						exitPoint = doorMapping[i + 1];
					}
					else if ((i % 2) != 0)
					{
						exitPoint = doorMapping[i - 1];
					}
				}
			}
			newLocationAdjust = exitPoint;
			if (exitPoint.X == 400 && exitPoint.Y == 0)
			{
				newLocationAdjust.Y += 100;
			}
			else if (exitPoint.X == 400 && exitPoint.Y == 800)
			{
				newLocationAdjust.Y -= 100;
			}
			else if (exitPoint.X == 0 && exitPoint.Y == 400)
			{
				newLocationAdjust.X += 100;
			}
			else if (exitPoint.X == 800 && exitPoint.Y == 400)
			{
				newLocationAdjust.X -= 100;
			}
			moveToRoom(newLocationAdjust);
		}
		else if (playerPosition.X >= 800)
		{
			doorLocation = FVector(800, 400, 0);
			doorLocation.Z += playerCurrentLoc.Z - playerZElevation.Z;

			for (int i = 0; i < doorMapping.Num(); i++)
			{
				if (doorLocation == doorMapping[i])
				{
					if ((i % 2) == 0)
					{
						exitPoint = doorMapping[i + 1];
					}
					else if ((i % 2) != 0)
					{
						exitPoint = doorMapping[i - 1];
					}
				}
			}
			newLocationAdjust = exitPoint;
			if (exitPoint.X == 400 && exitPoint.Y == 0)
			{
				newLocationAdjust.Y += 100;
			}
			else if (exitPoint.X == 400 && exitPoint.Y == 800)
			{
				newLocationAdjust.Y -= 100;
			}
			else if (exitPoint.X == 0 && exitPoint.Y == 400)
			{
				newLocationAdjust.X += 100;
			}
			else if (exitPoint.X == 800 && exitPoint.Y == 400)
			{
				newLocationAdjust.X -= 100;
			}
			moveToRoom(newLocationAdjust);
		}
	}
	else if ((playerPosition.Y <= 0 || playerPosition.Y >= 800) && (playerPosition.X <= 430 && playerPosition.X >= 370))
	{
		if (playerPosition.Y <= 0)
		{
			doorLocation = FVector(400, 0, 0);
			doorLocation.Z += playerCurrentLoc.Z - playerZElevation.Z;

			for (int i = 0; i < doorMapping.Num(); i++)
			{
				if (doorLocation == doorMapping[i])
				{
					if ((i % 2) == 0)
					{
						exitPoint = doorMapping[i + 1];
					}
					else if ((i % 2) != 0)
					{
						exitPoint = doorMapping[i - 1];
					}
				}
			}
			newLocationAdjust = exitPoint;
			if (exitPoint.X == 400 && exitPoint.Y == 0)
			{
				newLocationAdjust.Y += 100;
			}
			else if (exitPoint.X == 400 && exitPoint.Y == 800)
			{
				newLocationAdjust.Y -= 100;
			}
			else if (exitPoint.X == 0 && exitPoint.Y == 400)
			{
				newLocationAdjust.X += 100;
			}
			else if (exitPoint.X == 800 && exitPoint.Y == 400)
			{
				newLocationAdjust.X -= 100;
			}
			moveToRoom(newLocationAdjust);
		}
		else if (playerPosition.Y >= 800)
		{
			doorLocation = FVector(400, 800, 0);
			doorLocation.Z += playerCurrentLoc.Z - playerZElevation.Z;

			for (int i = 0; i < doorMapping.Num(); i++)
			{
				if (doorLocation == doorMapping[i])
				{
					if ((i % 2) == 0)
					{
						exitPoint = doorMapping[i + 1];
					}
					else if ((i % 2) != 0)
					{
						exitPoint = doorMapping[i - 1];
					}
				}
			}
			newLocationAdjust = exitPoint;
			if (exitPoint.X == 400 && exitPoint.Y == 0)
			{
				newLocationAdjust.Y += 100;
			}
			else if (exitPoint.X == 400 && exitPoint.Y == 800)
			{
				newLocationAdjust.Y -= 100;
			}
			else if (exitPoint.X == 0 && exitPoint.Y == 400)
			{
				newLocationAdjust.X += 100;
			}
			else if (exitPoint.X == 800 && exitPoint.Y == 400)
			{
				newLocationAdjust.X -= 100;
			}
			moveToRoom(newLocationAdjust);
		}
	}
}

void ADungeonsNDwellingsv4Pawn::moveToRoom(FVector newLocation)
{
	FVector playerNewLoc;
	bool isRoomNew = true;

	playerNewLoc = newLocation + playerZElevation;

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
	for (int i = 0; i < uniqueZTracker.Num(); i++)
	{
		if (uniqueZTracker[i] == (playerNewLoc.Z - playerZElevation.Z))
		{
			isRoomNew = false;
		}
	}
	if (isRoomNew == true)
	{
		uniqueZTracker.AddUnique(playerNewLoc.Z - playerZElevation.Z);
		UpdatePlayerCurrency();
		RegenHealth();
	}

	SetActorLocation(playerNewLoc, false);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions for controlling player taking damage/////////////////////////////////////////////////////////////////////////////////////////////////////
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
	FName gameOver;

	if (isDamageable == true)
	{
		playerHealth -= dmg;

		isDamageable = false;
	}

	gameOver = "GameOver";

	if (playerHealth <= 0)
	{
		UGameplayStatics::OpenLevel(GetWorld(), gameOver);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions for getting world values////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Pawn::GetLevelNumber()
{
	FString levelName = GetWorld()->GetMapName();;
	FString levelNameLeft;
	FString levelNameRight;
	FString splitCon = "-";

	levelName.Split(splitCon, &levelNameLeft, &levelNameRight, ESearchCase::CaseSensitive, ESearchDir::FromStart);

	levelNumber = FCString::Atoi(*levelNameRight);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions for getting key variables from other classes////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Pawn::getTotalOfDoors()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		totalDoorNum = ActorItr->getRunningTotal();
	}
}

FVector ADungeonsNDwellingsv4Pawn::getCurrentLocation()
{
	FVector currentLoc = GetActorLocation();

	return (currentLoc);
}

void ADungeonsNDwellingsv4Pawn::GetRoomPlacementModifier()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->getRoomPlacementModifier();
	}
}

FVector ADungeonsNDwellingsv4Pawn::GetPlayerZOffset()
{
	return (playerZElevation);
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

float ADungeonsNDwellingsv4Pawn::GetProjectileDamage()
{
	return (projectileDamage);
}

void ADungeonsNDwellingsv4Pawn::GetRoomCount()
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->getRoomCount();
	}
}

float ADungeonsNDwellingsv4Pawn::GetMoveSpeedModifier()
{
	return (enemySpeedReductionPercent);
}

void ADungeonsNDwellingsv4Pawn::GetDoorMappings()
{
	for (TActorIterator<ADoorPathingManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADoorPathingManager *Object = *ActorItr;
		doorMapping = ActorItr->GetDoorMappingArray();
	}
}

float ADungeonsNDwellingsv4Pawn::GetPlayerHealth()
{
	return (playerHealth);
}

int ADungeonsNDwellingsv4Pawn::GetPlayerCurrency()
{
	return (playerGold);
}

void ADungeonsNDwellingsv4Pawn::SetLevelComplete()
{
	isLevelComplete = true;
}

void ADungeonsNDwellingsv4Pawn::SetGameInstanceVariables()
{
	UDungeonsNDwellingsInstance* GI = Cast<UDungeonsNDwellingsInstance>(GetGameInstance());
	if (GI)
	{
		GI->GI_playerHealth = playerHealth;
		GI->GI_playerMaxHealth = playerMaxHealth;
		GI->GI_playerGold = playerGold;
		GI->GI_initialSpeed = initialSpeed;
		GI->GI_maxSpeed = maxSpeed;
		GI->GI_lifeSpan = lifeSpan;
		GI->GI_fireRate = FireRate;
		GI->GI_isGrowing = isGrowing;
		GI->GI_projectileScale = projectileScale;
		GI->GI_projectileDamage = projectileDamage;
		GI->GI_isSpawningSecondShot = isSpawningSecondShot;

		GI->GI_strBuffActive = strBuffActive;
		GI->GI_massBuffActive = massBuffActive;
		GI->GI_vigBuffActive = vigBuffActive;
		GI->GI_sacBuffActive = sacBuffActive;
		GI->GI_rateBuffActive = rateBuffActive;
		GI->GI_growthBuffActive = growthBuffActive;
		GI->GI_slowBuffActive = slowBuffActive;
		GI->GI_hyperBuffActive = hyperBuffActive;
		GI->GI_mnyShotBuffActive = mnyShotBuffActive;

		GI->GI_healthFromKills = healthFromKills;
		GI->GI_chanceToRecieveHealth = chanceToRecieveHealth;

		GI->GI_spawnChanceValue = spawnChanceValue;
		GI->GI_healthRegenValue = healthRegenValue;

		GI->GI_enemySpeedReductionPercent = enemySpeedReductionPercent;

		GI->GI_otherDmgChanges = otherDmgChanges;

		GI->GI_moneyDropModifier = moneyDropModifier;
	}
}

void ADungeonsNDwellingsv4Pawn::GetPlayerStatsFromGI()
{
	UDungeonsNDwellingsInstance* GI = Cast<UDungeonsNDwellingsInstance>(GetGameInstance());
	if (levelNumber != 1)
	{
		if (GI)
		{
			playerHealth = GI->GI_playerHealth;
			playerMaxHealth = GI->GI_playerMaxHealth;
			playerGold = GI->GI_playerGold;
			initialSpeed = GI->GI_initialSpeed;
			maxSpeed = GI->GI_maxSpeed;
			lifeSpan = GI->GI_lifeSpan;
			FireRate = GI->GI_fireRate;
			isGrowing = GI->GI_isGrowing;
			projectileScale = GI->GI_projectileScale;
			projectileDamage = GI->GI_projectileDamage;
			isSpawningSecondShot = GI->GI_isSpawningSecondShot;

			strBuffActive = GI->GI_strBuffActive;
			massBuffActive = GI->GI_massBuffActive;
			vigBuffActive = GI->GI_vigBuffActive;
			sacBuffActive = GI->GI_sacBuffActive;
			rateBuffActive = GI->GI_rateBuffActive;
			growthBuffActive = GI->GI_growthBuffActive;
			slowBuffActive = GI->GI_slowBuffActive;
			hyperBuffActive = GI->GI_hyperBuffActive;
			mnyShotBuffActive = GI->GI_mnyShotBuffActive;

			healthFromKills = GI->GI_healthFromKills;
			chanceToRecieveHealth = GI->GI_chanceToRecieveHealth;

			spawnChanceValue = GI->GI_spawnChanceValue;
			healthRegenValue = GI->GI_healthRegenValue;

			enemySpeedReductionPercent = GI->GI_enemySpeedReductionPercent;

			otherDmgChanges = GI->GI_otherDmgChanges;

			moneyDropModifier = GI->GI_moneyDropModifier;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








///Code snippets for reuse/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player Z Location is: %s"), *playerCoord));
	}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////