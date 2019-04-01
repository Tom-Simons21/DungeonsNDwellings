// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileGeneratorParent.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API ATileGeneratorParent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileGeneratorParent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<int32> getArrayOfDoors();
	int32 getRunningTotal();
	int32 getRoomCount();
	FVector getRoomPlacementModifier();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Properties")
	int32 roomXLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Properties")
	int32 roomYLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Properties")
	int32 tileSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Properties")
	int32 roomCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Properties")
	FVector roomPlacementModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
	int32 wallXLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
	int32 wallYLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
	int32 singleWallLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
	int32 wallNo1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
	int32 wallNo2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wall Properties")
	int32 wallNo3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
	int32 roomDoorCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
	TArray<int32> arrayOfDoors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
	int32 totalNumDoors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
	int32 minNumDoors;








	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
	TArray<int32> testingArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Properties")
	int32 testingTotal;
};
