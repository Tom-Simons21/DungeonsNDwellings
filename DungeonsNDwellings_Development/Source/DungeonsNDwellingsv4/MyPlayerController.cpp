// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"

//Basic functionality////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	isTextPopupActive = false; //text popup widget for items is on screen or not
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function to display item text widget to player/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::DisplayTextPopup()	//display item widget
{
	if (isTextPopupActive == false)		//check if its already on the screen
	{
		if (TextPopupClass != nullptr)	//check the class is valid
		{
			TextWidget = CreateWidget<UUserWidget>(GetWorld(), TextPopupClass); //assign widget
			if (TextWidget != nullptr)	//check widget is valid
			{
				TextWidget->AddToViewport();	//add to screen
				isTextPopupActive = true;		//set as active
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function to remove item text widget from viewport//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::RemoveTextPopup()	//remove item widget from screen
{
	if (TextWidget != nullptr)		//check is valid
	{
		TextWidget->RemoveFromViewport();	 //remove from screen
		isTextPopupActive = false;			 //set inactive - as it is not on screen anymore
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function to display pause menu to player on button press///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::OpenPauseMenu()		//controls pause menu
{
	if (PauseMenuClass != nullptr)		//check valid class
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuClass);	//assign widget
		if (PauseMenuWidget != nullptr)	//check valid widget
		{
			PauseMenuWidget->AddToViewport();	//add pause menu to screen
			bShowMouseCursor = true;			//give player control and visibilty for cursor
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function to return a text value - hello is default but it is always overridden in widget blueprint/////////////////////////////////////////////////////////////////////////////
FString AMyPlayerController::GetItemText()
{
	return ("Hello"); //collects item text, hello never shows was a simple debug value, also means never pass empty value
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


