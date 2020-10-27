// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "UObject/ConstructorHelpers.h"

APlayerHUD::APlayerHUD()
{
	//Find Object using file system
	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerHUDObject(TEXT("/Game/Widgets/PlayerHUDWidget"));

	PlayerHUDClass = PlayerHUDObject.Class;
	//Make sure the PlayerHUD class was found correctly
	if (PlayerHUDClass)
	{
		//Create a new HUD widget using the HUD object that was found.
		CurrentPlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);
		//Need to check that the widget was created successfully
		if (CurrentPlayerHUDWidget)
		{
			//Draw the hud to the player controllers viewport
			CurrentPlayerHUDWidget->AddToViewport();
			//Find the health bar and the ammo text block
			
			HealthProgressBar = Cast<UProgressBar>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("ProgHealthBar")));
			
			AmmoTextBlock = Cast<UTextBlock>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("TextAmmo")));
			
			KillsTextBlock = Cast<UTextBlock>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("TextKills")));
			DeathsTextBlock = Cast<UTextBlock>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("TextDeaths")));
			/*
			CrosshairImageBlock = Cast<UImage>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("ImgCrosshair")));
			*/
		}
	}
}

void APlayerHUD::SetPlayerHealthBarPercent(float Percent)
{
	if (HealthProgressBar) 
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void APlayerHUD::SetAmmoText(int32 RoundsRemaining, int32 MagazineSize)
{
	if (AmmoTextBlock)
	{
		AmmoTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%i/%i"), RoundsRemaining, MagazineSize)));
	}
}

void APlayerHUD::SetDeathsText(int32 Deaths)
{
	if (DeathsTextBlock)
	{
		DeathsTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Deaths: %i"), Deaths)));
	}
}

void APlayerHUD::SetKillsText(int32 Kills)
{
	if (KillsTextBlock)
	{
		KillsTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Kills: %i"), Kills)));
	}
}

void APlayerHUD::SetHideWidgets(bool bIsHidden)
{
	if (bIsHidden)
	{
		if (HealthProgressBar)
			HealthProgressBar->SetVisibility(ESlateVisibility::Hidden);
		
		if (AmmoTextBlock)
			AmmoTextBlock->SetVisibility(ESlateVisibility::Hidden);
		
		if (CrosshairImageBlock)
			CrosshairImageBlock->SetVisibility(ESlateVisibility::Hidden);

		//if (DeathsTextBlock)
			//DeathsTextBlock->SetVisibility(ESlateVisibility::Hidden);
			
	}
	else
	{
		if (HealthProgressBar)
		{
			HealthProgressBar->SetVisibility(ESlateVisibility::Visible);
		}
		
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Something went wrong with the hud."));
		}
		
		if (AmmoTextBlock)
			AmmoTextBlock->SetVisibility(ESlateVisibility::Visible);
		if (CrosshairImageBlock)
			CrosshairImageBlock->SetVisibility(ESlateVisibility::Visible);
		//if (DeathsTextBlock)
			//DeathsTextBlock->SetVisibility(ESlateVisibility::Visible);
		//Hack way. Having Gun C++ class instead of blueprints could fix this issue
		SetAmmoText(15, 15);
		SetPlayerHealthBarPercent(1.0f);
	}
}

