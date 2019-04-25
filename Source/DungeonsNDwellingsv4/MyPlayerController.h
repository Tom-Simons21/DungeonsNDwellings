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

private:
	/**********************************************************************PRIVATE VARIABLES*******************************************************************************/
	
	//Variable to check if text popup widget is currently on screen////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool isTextPopupActive;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	/**********************************************************************PUBLIC FUNCTIONS********************************************************************************/

	//widget functions, display, remove and pass variables respecitively to be used to add widgets to viewport/////////////////////////////////////////////////////////////
	void OpenPauseMenu();
	void DisplayTextPopup();
	void RemoveTextPopup();
	UFUNCTION(BlueprintPure, Category = "Item Popup")
		FString GetItemText();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};