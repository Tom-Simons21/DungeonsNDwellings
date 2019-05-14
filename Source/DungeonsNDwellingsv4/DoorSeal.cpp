// Fill out your copyright notice in the Description page of Project Settings.

#include "DoorSeal.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine.h"

// Sets default values
ADoorSeal::ADoorSeal()
{
	// Static reference to the mesh to use for the door
	static ConstructorHelpers::FObjectFinder<UStaticMesh>DoorSealAsset(TEXT("/Game/TwinStick/Meshes/Cuboid.Cuboid")); //mesh for doors

	// Create mesh component for the door
	CuboidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CuboidMesh0"));	//set up meshes for object
	RootComponent = CuboidMesh;
	CuboidMesh->SetStaticMesh(DoorSealAsset.Object);
	//CuboidMesh->SetupAttachment(RootComponent);
	CuboidMesh->BodyInstance.SetCollisionProfileName("Door"); //collision profile not properly set

	doorSealLocation = FVector(0, 0, 0);			//door parameters
	doorSealRotation = FRotator(0, 0, 0);
	doorSealScale = FVector(0.3, 0.95, 1.2);
}

//Functions for controlling door locations///////////////////////////////////////////////////////////////////////////////////////////////////
void ADoorSeal::updateVariables(FVector loc, FRotator rot, FVector sca) //modify door scale so it is appropriate for the position
{
	doorSealLocation = loc; //set location
	doorSealRotation = rot;	//set rotation
	doorSealScale = sca;	//set scale
}
void ADoorSeal::setLocation(FVector newLocation)
{
	doorSealLocation = newLocation;			//update door location, place it in the right spot
	SetActorLocation(doorSealLocation);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//GET and SET functions//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FVector ADoorSeal::getLocation()
{
	return (doorSealLocation);		//return the location
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////