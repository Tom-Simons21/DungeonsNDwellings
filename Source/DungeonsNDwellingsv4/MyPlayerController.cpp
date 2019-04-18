// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"



void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();


	isTextPopupActive = false;
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	if (ItemTextPopupClass != nullptr)
	{
		PopUpWidget = CreateWidget<UUserWidget>(GetWorld(), ItemTextPopupClass);

		if (PopUpWidget != nullptr)
		{
			PopUpWidget->AddToViewport();
		}
	}
	*/
}

void AMyPlayerController::DisplayTextPopup()
{
	if (isTextPopupActive == false)
	{
		if (TextPopupClass != nullptr)
		{
			TextWidget = CreateWidget<UUserWidget>(GetWorld(), TextPopupClass);

			if (TextWidget != nullptr)
			{
				TextWidget->AddToViewport();
				isTextPopupActive = true;
			}
		}
	}
}

void AMyPlayerController::RemoveTextPopup()
{
	
	if (TextWidget != nullptr)
	{
		TextWidget->RemoveFromViewport();
		isTextPopupActive = false;
	}

}

void AMyPlayerController::OpenPauseMenu()
{
	if (PauseMenuClass != nullptr)
	{
		PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuClass);

		if (PauseMenuWidget != nullptr)
		{
			PauseMenuWidget->AddToViewport();

			bShowMouseCursor = true;
		}
	}
}

FString AMyPlayerController::GetItemText()
{
	return ("Hello");
}



