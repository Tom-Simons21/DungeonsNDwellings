// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

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
			// Make a location for the new actor to spawn at (300 units above this actor)  
			FVector NewLocation = FVector(300.f, 300.f, 6050.f);

			// Spawn the new actor (Using GetClass() instead of AMySpawner so that if someone derives a new class  
			// from AMySpawner we spawn an instance of that class instead)  
			AInteractableObject* NewActor = GetWorld()->SpawnActor<AInteractableObject>(GetClass(), NewLocation, FRotator::ZeroRotator);

			// Housekeeping so that we dont spawn new actors forever  
			NewActor->Iteration = Iteration - 1;
			Iteration = 0; // stop ourselves spawning any more  
		}
	}
}

