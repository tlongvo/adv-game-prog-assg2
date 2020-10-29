// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuWidget.h"
#include "MainGameInstance.h"


bool UMainMenuWidget::Initialize()
{
	Super::Initialize();

	ButtonHost->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostButtonPressed);

	ButtonJoin->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinButtonPressed);

	ButtonQuit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonPressed);

	return true;
}


void UMainMenuWidget::OnHostButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Host Button Pressed"));
	UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->CreateSession(TEXT("Test Session"));
	}
}

void UMainMenuWidget::OnJoinButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Join Button Pressed"));
	UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->FindSession();
	}
}

void UMainMenuWidget::OnQuitButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Quit Button Pressed"));
	UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetGameInstance());
	if (GameInstance)
	{
	
	}
}