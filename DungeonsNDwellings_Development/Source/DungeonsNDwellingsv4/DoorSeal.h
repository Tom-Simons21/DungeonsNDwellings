// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorSeal.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API ADoorSeal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorSeal();

private:
	/**********************************************************************PRIVATE VARIABLES**************************************************************************/

	//Mesh for doors///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InteractableObject, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* CuboidMesh;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Positional properties for the door objects///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere)
		FVector doorSealLocation;				//location of seal
	UPROPERTY(EditAnywhere)
		FRotator doorSealRotation;				//rotation of seal
	UPROPERTY(EditAnywhere)
		FVector doorSealScale;					//scale of seal
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:	
	/**********************************************************************PUBLIC FUNCTIONS***************************************************************************/

	//variables for controlling door position in world/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void updateVariables(FVector location, FRotator rotation, FVector scale); //update the values appropriately
	void setLocation(FVector newLoc);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Get and Set functions////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FVector getLocation();			//pass back location
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};
