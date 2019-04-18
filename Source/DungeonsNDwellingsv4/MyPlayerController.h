// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONSNDWELLINGSV4_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pause Menu", Meta = (BlueprintProtected = "true"))
		TSubclassOf<class UUserWidget> PauseMenuClass;

	UPROPERTY()
		class UUserWidget* PauseMenuWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Popup", Meta = (BlueprintProtected = "true"))
		TSubclassOf<class UUserWidget> TextPopupClass;

	UPROPERTY()
		class UUserWidget* TextWidget;

	// Override BeginPlay()
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:

	bool isTextPopupActive;


	void DisplayTextPopup();
	void RemoveTextPopup();

	void OpenPauseMenu();

	UFUNCTION(BlueprintPure, Category = "Item Popup")
		FString GetItemText();
};