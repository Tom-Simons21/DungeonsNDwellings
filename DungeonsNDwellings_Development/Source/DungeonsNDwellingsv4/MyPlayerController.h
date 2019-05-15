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
		TSubclassOf<class UUserWidget> PauseMenuClass;	//pause menu class

	UPROPERTY()
		class UUserWidget* PauseMenuWidget;	//pause menu widget

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Popup", Meta = (BlueprintProtected = "true"))
		TSubclassOf<class UUserWidget> TextPopupClass;	//item popup name class

	UPROPERTY()
		class UUserWidget* TextWidget;	//item popup widget

	// Override BeginPlay()
	virtual void BeginPlay() override;

private:
	/**********************************************************************PRIVATE VARIABLES*******************************************************************************/
	
	//Variable to check if text popup widget is currently on screen////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool isTextPopupActive;	//is popup on screen
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	/**********************************************************************PUBLIC FUNCTIONS********************************************************************************/

	//widget functions, display, remove and pass variables respecitively to be used to add widgets to viewport/////////////////////////////////////////////////////////////
	void OpenPauseMenu();				//open + display pause menu
	void DisplayTextPopup();			//popup text at correct time
	void RemoveTextPopup();				//remove text popup the rest of the time
	UFUNCTION(BlueprintPure, Category = "Item Popup")
		FString GetItemText();			//the item text to be displayed
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};