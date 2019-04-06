// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileBoss.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Pawn.h"
#include "DungeonsNDwellingsv4Projectile.h"

// Sets default values
AProjectileBoss::AProjectileBoss()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>ProjectileBossMesh(TEXT("/Game/TwinStick/Meshes/ProjectileBoss.ProjectileBoss"));

	ProjectileBossComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BossMesh0"));
	RootComponent = ProjectileBossComponent;
	ProjectileBossComponent->SetStaticMesh(ProjectileBossMesh.Object);
	ProjectileBossComponent->SetupAttachment(RootComponent);
	ProjectileBossComponent->BodyInstance.SetCollisionProfileName("BlockAllDynamic");

	ProjectileBossComponent->SetSimulatePhysics(true);
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





	projectileBossContactDamage = 25;
	projectileBossHealth = 500;

	isBossActive = false;
}

void AProjectileBoss::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString actorName;
	float playerDmg;

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
			for (TActorIterator<ADungeonsNDwellingsv4Projectile> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
				ADungeonsNDwellingsv4Projectile *Object = *ActorItr;
				playerDmg = ActorItr->GetDamage();
			}
			this->TakeDamage(playerDmg);
		}
	}
}

// Called when the game starts or when spawned
void AProjectileBoss::BeginPlay()
{
	Super::BeginPlay();
	
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
	}
}

void AProjectileBoss::SetIsBossActive()
{
	isBossActive = true;
}

void AProjectileBoss::TakeDamage(float dmg)
{
	projectileBossHealth -= dmg;
}

