// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject_Spawner.h"

// Sets default values
AInteractableObject_Spawner::AInteractableObject_Spawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableObject_Spawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableObject_Spawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

