// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicSlugEnemy.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API ABasicSlugEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasicSlugEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InteractableObject, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* CylinderMeshComponent;

	/** Function to handle the enemy hitting something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FVector playerLocation;

	UPROPERTY(EditAnywhere)
		FVector slugLocation;
	UPROPERTY(EditAnywhere)
		FRotator slugRotation;
	UPROPERTY(EditAnywhere)
		FVector slugScale;

	UPROPERTY(EditAnywhere)
		bool isEnemyActive;

	float moveSpeed;

	UPROPERTY(EditAnywhere)
		float slugHealth;

	float slugDamage;

	






	float getZLocation();

	void setIsEnemyActive();

	void moveTowardsPlayer(float deltaTime);

	void takeDamage(float damage);
};
