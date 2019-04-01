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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InteractableObject, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* CuboidMesh;

	void TesterFunction();
};
