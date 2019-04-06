// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBoss.generated.h"

UCLASS()
class DUNGEONSNDWELLINGSV4_API AProjectileBoss : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBoss();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InteractableObject, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* ProjectileBossComponent;

	/** Function to handle the enemy hitting something */
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//Default positional properties for this boss////////////////////////////
	FRotator const rot = FRotator(0, 0, 0);								   //
	FVector const sca = FVector(1.3, 1.3, 0.6);							   //
	FVector loc = FVector(400, 400, 0);									   //
	float const zLoc = 32;												   //
	/////////////////////////////////////////////////////////////////////////


	float projectileBossContactDamage;

	float projectileBossHealth;

	UPROPERTY(EditAnywhere)
	bool isBossActive;


	void SetIsBossActive();

	void TakeDamage(float dmg);
};
