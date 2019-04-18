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

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", Meta = (BlueprintProtected = "true"))
		TSubclassOf<class UUserWidget> PlayerHUDClass;

	UPROPERTY()
		class UUserWidget* CurrentWidget;
};



