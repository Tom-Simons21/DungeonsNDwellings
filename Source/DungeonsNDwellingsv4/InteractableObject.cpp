// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "DungeonsNDwellingsv4Projectile.h"

// Sets default values
AInteractableObject::AInteractableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Static reference to the mesh to use for the Interactable object
	static ConstructorHelpers::FObjectFinder<UStaticMesh> InteractableObjectAsset(TEXT("/Game/TwinStick/Meshes/Cone.Cone"));

	// Create mesh component for the projectile sphere
	ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeMesh0"));
	RootComponent = ConeMesh;
	ConeMesh->SetStaticMesh(InteractableObjectAsset.Object);
	ConeMesh->SetupAttachment(RootComponent);
	SetActorScale3D(FVector(0.81, 0.81, 0.9));

	Iteration = 1;
}

// Called when the game starts or when spawned
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();

	setItemValue();
}

// Called every frame
void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Iteration > 0)
	{
		TimeToSpawn -= DeltaTime;
		if (TimeToSpawn < 0.f)
		{
			// Make a location for the new actor to spawn at
			FVector NewLocation = FVector(300.f, 300.f, 6050.f);
			
			//call function to spawn actor, passes back the value for iteration.
			Iteration = spawnInteractable(NewLocation);
		}
	}
}
/*
FVector AInteractableObject::updateSpawnLocation(int roomCount)
{
	///will be used later
}*/

int AInteractableObject::spawnInteractable(FVector spawnLoc)
{
		AInteractableObject* NewActor = GetWorld()->SpawnActor<AInteractableObject>(GetClass(), spawnLoc, FRotator::ZeroRotator);

		// Housekeeping so that we dont spawn new actors forever  
		NewActor->Iteration = Iteration - 1;
		Iteration = 0; // stop ourselves spawning any more 

		return Iteration;
}

void AInteractableObject::updateIsInteractable()
{
	isInteractable = false;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Function Called"));
	}
}

void AInteractableObject::getPlayerLocation(FVector playerPos)
{
	FVector playerLoc = playerPos;
	
}

void AInteractableObject::setItemValue()
{
	itemValue = FMath::RandRange(1, 3);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Item value was called"));
	}
}

void AInteractableObject::playerTakesItem()
{
	float objSpeed;
	float objSpeedMax;
	float objLifeSpan;

	if (itemValue == 1)
	{
		objSpeed = 3000;
		objSpeedMax = 3000;
		objLifeSpan = 15;

		callProjectileFunction(objSpeed, objSpeedMax, objLifeSpan);
	}
	else if (itemValue == 2)
	{
		objSpeed = 1000;
		objSpeedMax = 1000;
		objLifeSpan = 10;

		callProjectileFunction(objSpeed, objSpeedMax, objLifeSpan);
	}
	else if (itemValue == 3)
	{
		objSpeed = 500;
		objSpeedMax = 500;
		objLifeSpan = 5;

		callProjectileFunction(objSpeed, objSpeedMax, objLifeSpan);
	}
	else
	{
		objSpeed = 100;
		objSpeedMax = 100;
		objLifeSpan = 3;

		callProjectileFunction(objSpeed, objSpeedMax, objLifeSpan);
	}
}

void AInteractableObject::callProjectileFunction(float x, float y, float z)
{
	for (TActorIterator<ADungeonsNDwellingsv4Projectile> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ADungeonsNDwellingsv4Projectile *Object = *ActorItr;
		ActorItr->updateProperties(x, y, z);
	}
}

/*
void AInteractableObject::displayItemText()
{

}
*/
