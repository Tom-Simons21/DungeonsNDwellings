// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DungeonsNDwellingsv4GameMode.generated.h"

UCLASS(MinimalAPI)
class ADungeonsNDwellingsv4GameMode : public AGameModeBase
{
	GENERATED_BODY()

		virtual void BeginPlay() override;  //override begin play from base class

public:
	ADungeonsNDwellingsv4GameMode();

private:
	FString mapName;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", Meta = (BlueprintProtected = "true"))
		TSubclassOf<class UUserWidget> PlayerHUDClass; //class for HUD
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Loading", Meta = (BlueprintProtected = "true"))
		TSubclassOf<class UUserWidget> LoadingScreenClass; //class for Loading Screen

	UPROPERTY()
		class UUserWidget* CurrentWidget; //widget used for HUD
	UPROPERTY()
		class UUserWidget* LoadingWidget; //widget for loading screen

public:

	void DisplayLoadingScreen();	//display the loading screnn
	void RemoveLoadingScreen();		//remove loading screen				
};



