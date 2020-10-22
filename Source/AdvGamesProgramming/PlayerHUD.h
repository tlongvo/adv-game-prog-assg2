// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class ADVGAMESPROGRAMMING_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	APlayerHUD();

	/**
	 * Changes the percent of the progress bar health component to reflect the given percentage
	 * @param Percent: The percent that the health bar will be set to.
	 */
	void SetPlayerHealthBarPercent(float Percent);

	/**
	 * Hides or shows the widgets in the hud.
	 * @param bIsHidden: Whether the widgets should be hidden (true) or shown (false)
	 */
	void SetHideWidgets(bool bIsHidden);

private:
	TSubclassOf<class UUserWidget> PlayerHUDClass;
	UUserWidget* CurrentPlayerHUDWidget;
	UProgressBar* HealthProgressBar;
};
