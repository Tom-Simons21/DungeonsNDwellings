// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorSeal.h"
#include "DoorSealSpawner.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API ADoorSealSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorSealSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void spawnDoorSeals(FTransform spawnLocation, FVector loc, FRotator rot, FVector scale);

	void setupSpawns();

	int getRoomCount();

	void checkDoorStatus();

	void openDoors(int roomNum);

	int roomCount;

	FTransform position;

	UPROPERTY(EditAnywhere)
	TArray<ADoorSeal*> doorSealArray;
};
