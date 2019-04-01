// Fill out your copyright notice in the Description page of Project Settings.

#include "DoorSeal.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"

// Sets default values
ADoorSeal::ADoorSeal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Static reference to the mesh to use for the Interactable object
	static ConstructorHelpers::FObjectFinder<UStaticMesh>DoorSealAsset(TEXT("/Game/TwinStick/Meshes/Cuboid.Cuboid"));

	// Create mesh component for the projectile sphere
	CuboidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CuboidMesh0"));
	RootComponent = CuboidMesh;
	CuboidMesh->SetStaticMesh(DoorSealAsset.Object);
	CuboidMesh->SetupAttachment(RootComponent);
	CuboidMesh->BodyInstance.SetCollisionProfileName("Door");

	SetActorScale3D(FVector(1, 1, 1));
}

// Called when the game starts or when spawned
void ADoorSeal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoorSeal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorSeal::TesterFunction()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Called"));
	}
}

