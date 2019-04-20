// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemManager.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API AItemManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	/////////////////////////////////////////////////////////////////////////VARIABLES///////////////////////////////////////////////////////////////////////


	//Variables to control item tracking/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		int availableItemsCounter;
	UPROPERTY(EditAnywhere)
		int itemValue;									//this will randomly generate a number and then associate it with the relevant item

	UPROPERTY(EditAnywhere)
		TArray<FString> itemPool = { "  ", "Strength-1", "Strength-2", "Strength-3", "Vigor-1", "Vigor-2", "Vigor-3", "Masses-1", "Masses-2", "Masses-3" }; //0 is no item for the sake of checking the currentAvailableArray;
	UPROPERTY(EditAnywhere)
		TArray<FString> currentAvailableItems;
	UPROPERTY(EditAnywhere)
		TArray<FString> playerItems;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////FUNCTIONS///////////////////////////////////////////////////////////////////////

	
public:

	void SelectItem(FString objectName);

	void RerollItem(FString objectName);

	void AddItemToPlayer(FString objectName);

	UFUNCTION(BlueprintPure, Category = "Item Widget")
		TArray<FString> GetCurrentItems();
};
