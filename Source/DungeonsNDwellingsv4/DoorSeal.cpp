// Fill out your copyright notice in the Description page of Project Settings.

#include "DoorSeal.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"

// Sets default values
ADoorSeal::ADoorSeal()
{
	// Static reference to the mesh to use for the Interactable object
	static ConstructorHelpers::FObjectFinder<UStaticMesh>DoorSealAsset(TEXT("/Game/TwinStick/Meshes/Cuboid.Cuboid"));

	// Create mesh component for the projectile sphere
	CuboidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CuboidMesh0"));
	RootComponent = CuboidMesh;
	CuboidMesh->SetStaticMesh(DoorSealAsset.Object);
	CuboidMesh->SetupAttachment(RootComponent);
	CuboidMesh->BodyInstance.SetCollisionProfileName("Door");

	doorSealLocation = FVector(0, 0, 0);
	doorSealRotation = FRotator(0, 0, 0);
	doorSealScale = FVector(0.3, 0.95, 1.2);
}

//Functions for controlling door locations///////////////////////////////////////////////////////////////////////////////////////////////////
void ADoorSeal::updateVariables(FVector loc, FRotator rot, FVector sca)
{
	doorSealLocation = loc;
	doorSealRotation = rot;
	doorSealScale = sca;
}
void ADoorSeal::setLocation(FVector newLocation)
{
	doorSealLocation = newLocation;
	SetActorLocation(doorSealLocation);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//GET and SET functions//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FVector ADoorSeal::getLocation()
{
	return (doorSealLocation);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////