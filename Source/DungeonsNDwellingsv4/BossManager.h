// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossManager.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API ABossManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Projectile Boss Spawn Point///////////////////////////////////////////////////////////
	FVector const spawnLocation = FVector(400, 400, 10032);								  //	
	FRotator const spawnRotation = FRotator(0, 0, 0);									  //
	FVector const spawnScale = FVector(1.3, 1.3, 0.6);									  //
	FTransform const spawnTrans = FTransform(spawnRotation, spawnLocation, spawnScale);   //
	////////////////////////////////////////////////////////////////////////////////////////

	int roomCount;

	//These values will be constant across all classes//////////////////////////////////////
	float const playerOffset = 22;														  //
	float const roomOffset = 2000;														  //
	////////////////////////////////////////////////////////////////////////////////////////
	//Constant Z Offset for this boss///////////////////////////////////////////////////////
	float const projectileBossOffset = 32;												  //
	////////////////////////////////////////////////////////////////////////////////////////




	void ActivateBoss(FVector playerLoc);

	void GetRoomCount();
};
