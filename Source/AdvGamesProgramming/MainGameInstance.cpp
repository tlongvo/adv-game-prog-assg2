// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Interfaces/OnlineSessionInterface.h"


UMainGameInstance::UMainGameInstance(const FObjectInitializer& ObjectInitialize)
{

	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuObject(TEXT("/Game/Widgets/MainMenuWidget"));

	MainMenuWidgetClass = MainMenuObject.Class;

}

void UMainGameInstance::Init()
{
	Subsystem = IOnlineSubsystem::Get();
	
	if (Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Online Subsystem: %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();

		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnCreateSessionComplete);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to find Online Subsystem"));
	}

	CreateSession(TEXT("Test Session"));
}


void UMainGameInstance::CreateSession(FName SessionName)
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsLANMatch = true;
		SessionSettings.NumPublicConnections = 3;
		SessionSettings.bShouldAdvertise = true;
		SessionInterface->CreateSession(0, SessionName, SessionSettings);

	}
}

void UMainGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Created Successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session was not Created"));
	}


}

void UMainGameInstance::LoadMenu()
{
	Menu = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);

	if (Menu)
	{
		Menu->AddToViewport();

		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetWidgetToFocus(Menu->TakeWidget());

		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}
