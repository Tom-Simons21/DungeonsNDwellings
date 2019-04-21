// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InteractableObject.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableObjectManager.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API AInteractableObjectManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableObjectManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:

	//Variables to track player item relations///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float const minimumDistanceToInteract = 120;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FVector objectSpawn = FVector(400, 400, 50);
	FRotator const objectRotator = FRotator(0, 0, 0);
	FVector const objectScale = FVector(1, 1, 1);

	int itemSpawnedCounter;

	float distance;

	UPROPERTY(EditAnywhere)
		TArray<AInteractableObject*> interactableObjectArray;


	void SpawnInteractableObject();

public:

	int roomCount;
	FVector roomPlacementModifier;

	void SpawnInteractableOnComplete();
	bool CheckDistanceFromPlayer();

	bool Reroll();
	void Claim();

	void GetRoomCount();
	void GetRoomPlacementModifier();
};
