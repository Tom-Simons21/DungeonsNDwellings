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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/Cube.Cube")); //select the asset for the player
	// Create the mesh component
	PlayerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));		//setup mesh object
	RootComponent = PlayerMeshComponent;														//setup root
	PlayerMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);			//setup collision
	PlayerMeshComponent->SetStaticMesh(ShipMesh.Object);										//attach asset to mesh

	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;								//audio

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
	CameraComponent->FieldOfView = 66;														//field of view to be used
	CameraComponent->AddRelativeLocation(FVector(0, 0, 0));						//relative location directly above
	CameraComponent->SetAspectRatio(1.0);										

	// Movement
	MoveSpeed = 1000.0f;								

	playerZElevation = FVector(0, 0, 22);					//player default elevation
	// Weapon
	GunOffset = FVector(36.f, 0.f, 0.f);
	bCanFire = true;

	//set values to be used for projectile
	initialSpeed = 250;
	maxSpeed = 250;
	lifeSpan = 2;									//explained on projectile
	FireRate = 0.3;
	projectileScale = FVector(1, 1, 1);
	isGrowing = false;

	//projectile damage values
	projectileDefaultDamage = 10;		
	projectileDamage = 10;

	//set values for player stats
	playerMaxHealth = 100;
	playerHealthDefault = 100;
	playerHealth = playerHealthDefault;
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
	rateBuffActive = false;
	growthBuffActive = false;
	slowBuffActive = false;
	hyperBuffActive = false;
	mnyShotBuffActive = false;

	//value for multishot runs from 4 - 1, calculated by chance at rolling a 1 so 4 = 25% chance
	spawnChanceValue = 0;
	
	//amount of health being regen'd per room cleared
	healthRegenValue = 0;

	//maintain correct dmg stats
	hyperModePercent = 1;
	otherDmgChanges = 0;

	//set enemy speed modifier
	enemySpeedReductionPercent = 1;

	//checks if the boss has been killed
	isLevelComplete = false;
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
	if (doorMapping.Num() < 1)  //load order causes issues with accessing this array - get it every tick until it fills
	{
		GetDoorMappings();		//gets the array
	}

	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);			//get forward input = +/-
	const float RightValue = GetInputAxisValue(MoveRightBinding);				//get right input = +/-

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
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);			//forward vector for projectile direction 
	const float FireRightValue = GetInputAxisValue(FireRightBinding);				//right vector for projectile direction
	const FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);

	// Try and fire a shot
	FireShot(FireDirection);			//shoot, pass direction vector

	CalcPlayerLocation();				//gets players location
}

void ADungeonsNDwellingsv4Pawn::FireShot(FVector FireDirection)
{
	FVector spawnLocation;
	isSpawningSecondShot = false; //by default only spawns 1 shot

	// If it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			if (massBuffActive == true)	//check if double shot buff has started
			{
				isSpawningSecondShot = SpawnAdditionalShots(FireDirection); //spawns an additional shot (chance to) if buff is active
			}

			const FRotator FireRotation = FireDirection.Rotation();	//setup fire rotation
			// Spawn projectile at an offset from this pawn
			if (isSpawningSecondShot == true)		//this is true if the chance returns true, always a chance buff increases with buff count
			{
				spawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset - FVector(0, 15, 0)); //location for projectile to spawn when 2 shots are spawning, needs additional offset
			}
			else
			{
				spawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset); //position to spawn when only 1 projectile
			}
			const FVector Scale = FVector(1, 1, 1);	//scale
			const FTransform SpawnPosition = FTransform(FireRotation, spawnLocation, Scale); //spawn transform

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile
				ADungeonsNDwellingsv4Projectile* projectileActor = World->SpawnActorDeferred<ADungeonsNDwellingsv4Projectile>(ADungeonsNDwellingsv4Projectile::StaticClass(), SpawnPosition);
				projectileActor->UpdateProperties(initialSpeed, maxSpeed, lifeSpan, isGrowing, projectileScale); //updates properties on each spawn with any variations
				projectileActor->FinishSpawning(SpawnPosition); //variance done here so it applies pre spawn, not the second after spawn
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ADungeonsNDwellingsv4Pawn::ShotTimerExpired, FireRate); //timer to track when player can shoot again

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
	
	//Initialise all variables from external classes asap//////////////////////////////////////////////
	GetLevelNumber();						//gets the level player is on
	GetPlayerStatsFromGI();					//gets stats from game instance - unless is level 1
	GetRoomPlacementModifier();				//gets distance between rooms
	GetRoomCount();							//gets number of rooms
	GetDoorMappings();						//gets array of door pairings
	///////////////////////////////////////////////////////////////////////////////////////////////////

	isDamageable = false;						//player is invulnerable at spawn time - just incase tbh
	GetWorldTimerManager().SetTimer(hitControlTimer, this, &ADungeonsNDwellingsv4Pawn::MakeDamageable, 1.5f, true, 2.0f); //if player becomes invulnerable they will be made damageable again in 1.5s
	SetActorLocation(playerStartPoint, false); //spawn actor at start point
	SetActorScale3D(playerScale);	//set scale
}

void ADungeonsNDwellingsv4Pawn::OnInteract() //player takes item
{
	for (TActorIterator<AInteractableObjectManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AInteractableObjectManager *Object = *ActorItr;
		ActorItr->Claim();	//call to interactable object manager for functionality
	}
}
void ADungeonsNDwellingsv4Pawn::OnReroll() //player requests new item
{
	bool isRerolled = false;	

	if (playerGold >= 2)	//player must pay 2 gold to reroll
	{
		for (TActorIterator<AInteractableObjectManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AInteractableObjectManager *Object = *ActorItr;
			isRerolled = ActorItr->Reroll();	//manager handles functionality
		}
		if (isRerolled == true)
		{
			playerGold -= 2;			//if reroll is successful player pays the 2 gold, effectively it checks if the item available is the last item in the pool
		}
	}
}
void ADungeonsNDwellingsv4Pawn::OnPauseGame()	//pause the game
{
	UWorld* const World = GetWorld();
	UGameplayStatics::SetGamePaused(World, true);		//set game to paused
	if (GEngine)
	{
		AMyPlayerController* const MyPlayer = Cast<AMyPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));	//call to player controller
		if (MyPlayer != NULL)
		{
			MyPlayer->OpenPauseMenu();		//call to open pause menu function - displays pause menu
		}
	}
}
void ADungeonsNDwellingsv4Pawn::OnNextLevel()	//go to the next level
{
	GetLevelNumber();
	FString pathPrefix = "/Game/TwinStickCPP/Maps/";
	FString nextLevel;				//the number of the next level as a string
	FString nextLevelName;			//the name of the next level
	FName name;						//name version of string

	if (levelNumber + 1 < 6)	//check if the next level is within the current level pool
	{
		nextLevel = FString::FromInt(levelNumber + 1);	//create a string value that is current level number + 1
		nextLevelName = levelPrefix + nextLevel;		//append the +1 version to the level prefix

		nextLevelName = pathPrefix + nextLevelName;

		name = FName(*nextLevelName);					//create a name variant of the level string
	}
	else
	{
		name = "WinGame";								//if all levels are done open win menu
	}
	if (isLevelComplete)								//if not last level but level is complete
	{
		for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
			AItemManager *Object = *ActorItr;
			ActorItr->TransitionToNewLevel();			//trigger transition level functions
		}
		SetGameInstanceVariables();						//pass all consistent values to game instance
		UGameplayStatics::OpenLevel(GetWorld(), name);	//open next level
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control player currency and externals///////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Pawn::UpdatePlayerCurrency()	//increases player gold over time
{
	int goldBonus;									//amount of gold to gain 
	int interest = playerGold / 10;					//interest that will be gained

	if (interest > 5)								//make sure there is a max set on interest
	{
		interest = 5;								//max is 5
	}
	if (playerHealth < 100)							//if player loses health lose win streak
	{
		winStreak = 0;
	}
	if (playerHealth >= 25)							//if player restores health passed 25 lose lose streak
	{
		loseStreak = 0;
	}

	if (winStreak > 1)								//to be improved on current bonuses only +1 per turn
	{
		winStreak = 1;
	}
	else if (loseStreak > 1)
	{
		loseStreak = 1;
	}
	
	goldBonus = goldToAdd + interest + winStreak + loseStreak;	//amount to add
	playerGold += goldBonus;									//add gold to player
	goldToAdd = 1;												//gold to add is set at end so no gold is granted for room 1

	if (playerHealth == 100)
	{
		winStreak++;											//gain win streak gold for staying full health
	}
	else if (playerHealth < 25)
	{
		loseStreak++;											//gain lose streak gold for being below 25 health
	}	
	if (playerGold > 100)
	{
		playerGold = 100;										//max gold = 100
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions for item management and buff/debuff application////////////////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Pawn::SetStrBuff()
{
	strBuffActive = true;								//set buffs to active
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

void ADungeonsNDwellingsv4Pawn::ModifyPlayerDamage(bool isABuff, float damageMultiplier)	//damage mod
{
	bool strBuffMaxed = false;		//maxed is false by default

	for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AItemManager *Object = *ActorItr;
		strBuffMaxed = ActorItr->IsStrBuffMaxed();			//check if str buff is maxed
	}
	if (isABuff == false) //it is a debuff
	{
		if (strBuffMaxed != true)
		{
			projectileDamage = projectileDamage * damageMultiplier;		//debuff to damage applied - str buff cures the debuff
		}
	}
	if (isABuff == true) //it is a buff
	{
		if (strBuffMaxed == true)
		{
			projectileDamage = (projectileDefaultDamage + otherDmgChanges) * damageMultiplier; //apply max buff
		}
		else if (strBuffActive == true)
		{
			projectileDamage = projectileDamage * damageMultiplier; //apply buff 
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyProjectileSpawnChance(bool isABuff, int spawnChanceModifier)	//chance to spawn second shot
{
	if (massBuffActive == true)
	{
		spawnChanceValue = spawnChanceModifier;	//set chance
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyPlayerHealth(bool isABuff, float healthIncrease, bool isHealthRegening, float healthRegenAmount)	//increase health + give regen
{
	bool vigBuffMaxed = false;
	float currentHealthLost = playerMaxHealth - playerHealth;	//get players current health

	for (TActorIterator<AItemManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AItemManager *Object = *ActorItr;
		vigBuffMaxed = ActorItr->IsVigBuffMaxed();	//check if buff is maxed
	}
	if (currentHealthLost < 0)
	{
		currentHealthLost = 0;	//cannot have lost less than 0 health
	}
	if (isABuff == true)
	{
		if (vigBuffActive == true)
		{
			playerMaxHealth = playerHealthDefault + healthIncrease;	//increase max health
			playerHealth = playerHealthDefault + healthIncrease - currentHealthLost; //this is not a full heal so the player gains health but keeps however much damage had been taken
			if (isHealthRegening == true)		//is health regening
			{
				healthRegenValue = healthRegenAmount; //if yes set amount per room
			}
		}
	}
	if (isABuff == false)			//is a debuff
	{
		if (vigBuffMaxed != true)		//max buff cures
		{
			playerHealth = playerHealth * healthIncrease;	//negative health 
			playerMaxHealth = playerHealth;	//player max health also reduced
		}
		if (isHealthRegening == true)	
		{
			healthRegenValue = (playerHealth / 10) * (-1);	//player loses 10% health per room
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyPlayerKillBonuses(bool isABuff, float healthToRecieve, int chanceToRecieve)	//chance to recieve bonus on kill
{
	if (isABuff == true)
	{
		if (sacBuffActive == true)
		{
			healthFromKills = healthToRecieve;				//amount of health to recieve
			chanceToRecieveHealth = chanceToRecieve;		//chance to recieve
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyPlayerFireRate(bool isABuff, float fireRate) //modifies firerate
{
	if (isABuff == true)
	{
		if (rateBuffActive == true)
		{
			FireRate = fireRate; //increase fire rate
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyPlayerProjectileStyle(bool isABuff, float initSpeed, float topSpeed, float lifeTime, bool isGrow, FVector scale)	//apply growth buff
{
	if (isABuff == true)
	{
		if (growthBuffActive == true)
		{
			initialSpeed = initSpeed;			//modify typical values
			maxSpeed = topSpeed;
			lifeSpan = lifeTime;
			isGrowing = isGrow;
			projectileScale = scale;
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyEnemyMoveSpeed(bool isABuff, float reductionPercent)	//debuff enemies via slow effect
{
	if (isABuff == true)
	{
		if (slowBuffActive == true)
		{
			enemySpeedReductionPercent = reductionPercent;	//percent to slow them by
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyMoneyDropChance(bool isABuff, float dropChanceIncrease)			//chance to drop additional loot
{
	if (isABuff == true)
	{
		if (mnyShotBuffActive == true)
		{
			moneyDropModifier = dropChanceIncrease;		//money drop chance set

			for (TActorIterator<AEnemySpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				AEnemySpawner *Object = *ActorItr;
				ActorItr->SetMoneyDropChance(moneyDropModifier); //modify enemy manager etc
			}
		}
	}
}

void ADungeonsNDwellingsv4Pawn::ModifyHyperModePercent(float hyperPercent) //hyper mode
{
	hyperModePercent = hyperPercent; //set hyper mode percent
}

void ADungeonsNDwellingsv4Pawn::ActivateHyperMode() //activate hyper mode
{
	float healthLost;				//tracks health lost
	float highHealthPenalty;		//penalty for being above 50% health
	float dmgIncrease = 0;			//damage increase due to buff

	healthLost = playerMaxHealth - playerHealth;	//calc health lost
	if (healthLost < 0)
	{
		healthLost = 0;				//cannot have lost < than 0 health not technically possible
	}
	else if (healthLost > (playerMaxHealth / 2))	//check if health loss more than half
	{
		dmgIncrease = (healthLost - (playerMaxHealth / 2)) * hyperModePercent;	//add damage
	}
	else if (healthLost < (playerMaxHealth / 2)) //if no
	{
		highHealthPenalty = ((playerMaxHealth / 2) - healthLost) + 10; //damage penalty
		dmgIncrease = (highHealthPenalty / 20) * (-1);		//apply penalty
	}
	if (hyperBuffActive == true)
	{
		if (otherDmgChanges < 0)		//track damage that is being added through this function
		{
			otherDmgChanges = otherDmgChanges * (-1);	//must be a positive
		}
		projectileDamage += otherDmgChanges;	//restore projectile damage to pre-debuff
		projectileDamage += dmgIncrease;		//then apply hyper mode affect
		otherDmgChanges += dmgIncrease;			//then track damage changes
	} //this area still needs a little work - works but not quite as intended
}

bool ADungeonsNDwellingsv4Pawn::SpawnAdditionalShots(FVector FireDirection)	//spawn additional shot 
{
	bool secondShotSpawned = false;

	const FRotator FireRotation = FireDirection.Rotation();
	const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset + FVector(0, 15, 0)); //offset shot to one side a little
	const FVector Scale = FVector(1, 1, 1);
	const FTransform SpawnPosition = FTransform(FireRotation, SpawnLocation, Scale); //set up spawn transform same as for standard shot
	int spawnChance = FMath::RandRange(1, spawnChanceValue);	//chance to spawn between 1 - 4, 1 - 2, 1 - 1 currently, if = 1 then second shot spawns

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		if (spawnChance == 1) //as described above
		{
			secondShotSpawned = true;
			// spawn the projectile
			ADungeonsNDwellingsv4Projectile* projectileActor = World->SpawnActorDeferred<ADungeonsNDwellingsv4Projectile>(ADungeonsNDwellingsv4Projectile::StaticClass(), SpawnPosition);
			projectileActor->UpdateProperties(initialSpeed, maxSpeed, lifeSpan, isGrowing, projectileScale);
			projectileActor->FinishSpawning(SpawnPosition); //spawn same as with standard
		}
	}
	return (secondShotSpawned); //tell spawning function the second shot spawned so offset first shot
}

void ADungeonsNDwellingsv4Pawn::RegenHealth()
{
	if (playerHealth < playerMaxHealth)	//player health cannot be max to regen
	{
		if (vigBuffActive == true)
		{
			playerHealth += healthRegenValue;	//regen health
			if (playerHealth > playerMaxHealth)	//player health cannot be above max
			{
				playerHealth = playerMaxHealth;	//if above max set to max
			}
		}
	}
}

void ADungeonsNDwellingsv4Pawn::GainHealthOnKill() //gain health when an enemy dies
{
	int chanceOfHealth;	//chance to gain
	chanceOfHealth = FMath::RandRange(1, chanceToRecieveHealth);	// chance is chance to get a 1 between 1 and X so 1 and 20 chance is 5%
	if (chanceOfHealth == 1)
	{
		playerHealth += healthFromKills; //give player health if chance correct
	}
}

//Functions for modifying properties of the player projectile object///////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Pawn::UpdateProjectileValues(float initSpeed, float topSpeed, float lifeTime, bool growing, FVector scale)
{
	initialSpeed = initSpeed;
	maxSpeed = topSpeed;			//set projectile stats
	lifeSpan = lifeTime;
	isGrowing = growing;
	projectileScale = scale;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions to control and track player location and movement within a level///////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Pawn::CalcPlayerLocation()
{
	for (TActorIterator<AInteractableObject> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AInteractableObject *Object = *ActorItr;
		ActorItr->GetPlayerDistance();	//how far from interactable object am i
	}
	CheckPlayerLocation(); //checking and passing the players location
}

void ADungeonsNDwellingsv4Pawn::CheckPlayerLocation()
{
	FVector playerPosition = GetActorLocation(); //gets current position
	FVector doorLocation; //holds location of a door

	if ((playerPosition.X <= 0 || playerPosition.X >= 800) && (playerPosition.Y <= 430 && playerPosition.Y >= 370)) //is player between the door at bottom or the door at the top
	{
		if (playerPosition.X <= 0)			//door at the bottom
		{
			doorLocation = FVector(0, 400, 0);	//set door location
			doorLocation.Z += playerPosition.Z - playerZElevation.Z; //modify it for the room the player is in
			SetLocationAdjustment(doorLocation);	//pass the confirmed door location
		}
		else if (playerPosition.X >= 800)		//door at the top
		{
			doorLocation = FVector(800, 400, 0);
			doorLocation.Z += playerPosition.Z - playerZElevation.Z;
			SetLocationAdjustment(doorLocation); //same as above
		}
	}
	else if ((playerPosition.Y <= 0 || playerPosition.Y >= 800) && (playerPosition.X <= 430 && playerPosition.X >= 370))	//is player between door on left or door on the right
	{
		if (playerPosition.Y <= 0)	//door on the left
		{
			doorLocation = FVector(400, 0, 0);
			doorLocation.Z += playerPosition.Z - playerZElevation.Z;
			SetLocationAdjustment(doorLocation); //same as above
		}
		else if (playerPosition.Y >= 800)	//door on the right
		{
			doorLocation = FVector(400, 800, 0);
			doorLocation.Z += playerPosition.Z - playerZElevation.Z;
			SetLocationAdjustment(doorLocation);	//same as above
		}
	}
}

void ADungeonsNDwellingsv4Pawn::SetLocationAdjustment(FVector doorLocation)	//use door location to find where the player should move to
{
	FVector newLocationAdjust;	//new player location

	for (int i = 0; i < doorMapping.Num(); i++)	//check against all doors mapped
	{
		if (doorLocation == doorMapping[i])	//if door matches a door in the mapping array, which it will
		{
			if ((i % 2) == 0)	//if it is divisible by 2 with no remainder, it is even, then the next door is where we go, evens match to the next odd
			{
				exitPoint = doorMapping[i + 1];	//set exit point as next value
			}
			else if ((i % 2) != 0)	//if the index is uneven
			{
				exitPoint = doorMapping[i - 1];	//the previous even value is needed for the exit point
			}
		}
	}
	newLocationAdjust = exitPoint;						//assign
	if (exitPoint.X == 400 && exitPoint.Y == 0)			//adjust exit point so that player doesnt spawn in a doorway and move again
	{
		newLocationAdjust.Y += 100;
	}
	else if (exitPoint.X == 400 && exitPoint.Y == 800)	//adjust exit point so that player doesnt spawn in a doorway and move again
	{
		newLocationAdjust.Y -= 100;
	}
	else if (exitPoint.X == 0 && exitPoint.Y == 400)	//adjust exit point so that player doesnt spawn in a doorway and move again
	{
		newLocationAdjust.X += 100;
	}
	else if (exitPoint.X == 800 && exitPoint.Y == 400)	//adjust exit point so that player doesnt spawn in a doorway and move again
	{
		newLocationAdjust.X -= 100;
	}
	MoveToRoom(newLocationAdjust); //move the player to the point set
}

void ADungeonsNDwellingsv4Pawn::MoveToRoom(FVector newLocation) //move player appropriately
{
	FVector playerNewLoc;	//new location for player
	bool isRoomNew = true;	//check if the player is moving to a new room

	playerNewLoc = newLocation + playerZElevation;	//add the elevation to the player so they are not in the floor

	for (TActorIterator<AEnemySpawner> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		AEnemySpawner *Object = *ActorItr;
		ActorItr->ActivateEnemies(playerNewLoc);	//activate the enemies in the room the player is being placed in
	}
	for (TActorIterator<ABossManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ABossManager *Object = *ActorItr;
		ActorItr->ActivateBoss(playerNewLoc);		//activate the boss in the room the player is moving to
	}
	for (int i = 0; i < uniqueZTracker.Num(); i++)	//track unique rooms the player enters, bonuses are only given when entering a new room
	{
		if (uniqueZTracker[i] == (playerNewLoc.Z - playerZElevation.Z))	//check if the room the player is in matches a coord in unique room array
		{
			isRoomNew = false;	//if found room wasnt new
		}
	}
	if (isRoomNew == true) //if room is new
	{
		uniqueZTracker.AddUnique(playerNewLoc.Z - playerZElevation.Z);	//add the room player is in to unique array
		UpdatePlayerCurrency();											//if room is new player can gain currency
		RegenHealth();													//if room is new player can regen if active
	}

	FVector test = FVector(400, 400, 2022);		//just a test value used cos launch version is shit and breaks randomly

	SetActorLocation(playerNewLoc, false);	//place player in new location
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions for controlling player taking damage/////////////////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Pawn::MakeDamageable()	//makes the player damageable
{
	bool makeDamageable = true;	//player is damageable

	ApplyInvulnerability(makeDamageable);	//makes player invulnerable
}
void ADungeonsNDwellingsv4Pawn::ApplyInvulnerability(bool isD)	//makes player invulnerable when they take damage
{
	isDamageable = isD;	
}
void ADungeonsNDwellingsv4Pawn::PlayerTakeDamage(float dmg)	//player takes damage
{
	FName gameOver = "GameOver";	//assign name for game over level

	if (isDamageable == true)	//can only be damaged if isdamageable
	{
		playerHealth -= dmg;	//take damage
		ActivateHyperMode();	//hyper mode changes whenever damage is taken
		isDamageable = false;	//play gains invulnerability when takes damage
	}
	if (playerHealth <= 0)	//if health is below 0
	{
		UGameplayStatics::OpenLevel(GetWorld(), gameOver);	//game over
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions for getting world values////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADungeonsNDwellingsv4Pawn::GetLevelNumber()		//get number of current level
{
	FString levelName = GetWorld()->GetMapName();		//get map name
	FString levelNameLeft;								//left side of level name string
	FString levelNameRight;								//right side will hold level number
	FString splitCon = "-";								//delimiter

	levelName.Split(splitCon, &levelNameLeft, &levelNameRight, ESearchCase::CaseSensitive, ESearchDir::FromStart);	//split the string
	levelNumber = FCString::Atoi(*levelNameRight); //level number is the right side value
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Functions for getting key variables from other classes////////////////////////////////////////////////////////////////////////////////////////
FVector ADungeonsNDwellingsv4Pawn::GetPlayersCurrentLocation()	//gets the players location
{
	FVector currentLoc = GetActorLocation();	
	return (currentLoc);	//return location
}

void ADungeonsNDwellingsv4Pawn::GetRoomPlacementModifier()		//get distance between rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomPlacementModifier = ActorItr->GetRoomPlacementModifier();
	}
}

FVector ADungeonsNDwellingsv4Pawn::GetPlayerZOffset()	//return player elevation off of ground
{
	return (playerZElevation);
}

float ADungeonsNDwellingsv4Pawn::GetProjectileDamage() //return the damage the projectile does
{
	return (projectileDamage);
}

void ADungeonsNDwellingsv4Pawn::GetRoomCount()	//get number of rooms
{
	for (TActorIterator<ATileGeneratorParent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ATileGeneratorParent *Object = *ActorItr;
		roomCount = ActorItr->GetRoomCount();
	}
}

float ADungeonsNDwellingsv4Pawn::GetMoveSpeedModifier()	//return the speed modifier
{
	return (enemySpeedReductionPercent);
}

void ADungeonsNDwellingsv4Pawn::GetDoorMappings() //get the array of door mappings
{
	for (TActorIterator<ADoorPathingManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADoorPathingManager *Object = *ActorItr;
		doorMapping = ActorItr->GetDoorMappingArray();
	}
}

float ADungeonsNDwellingsv4Pawn::GetPlayerHealth() //returns the players health
{
	return (playerHealth);
}

int ADungeonsNDwellingsv4Pawn::GetPlayerCurrency() //returns the players currency
{
	return (playerGold);
}

void ADungeonsNDwellingsv4Pawn::SetLevelComplete() //sets level to complete
{
	isLevelComplete = true;
}

void ADungeonsNDwellingsv4Pawn::SetGameInstanceVariables() //passes consistent variables to game instance at end of level
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

void ADungeonsNDwellingsv4Pawn::GetPlayerStatsFromGI() //gets all values from game instance at start of level, except level 1
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