// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


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
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnDestroySessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMainGameInstance::OnFindSessionComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnJoinSessionComplete);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to find Online Subsystem"));
	}
	//CreateSession(TEXT("Test Session"));
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


void UMainGameInstance::CreateSession(FName SessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Session"));
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsLANMatch = true;
		SessionSettings.NumPublicConnections = 3;
		SessionSettings.bShouldAdvertise = true;

		SessionInterface->CreateSession(0, SessionName, SessionSettings);
	}
}

void UMainGameInstance::DestroySession(FName SessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Destroying Session"));
	if (SessionInterface.IsValid()) //If session exists
	{
		SessionInterface->DestroySession(SessionName);
	}
}

void UMainGameInstance::FindSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Finding Sessions"));
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		SessionSearch->bIsLanQuery = true;
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
		//Delegate OnFindSessionsComplete will be triggered
		//which is where joining the found session occurs
	}
}

void UMainGameInstance::JoinSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Joining Session"));
	if (SessionInterface.IsValid() && SessionSearch.IsValid())
	{
		//If there is a session in the list to join.
		if (SessionSearch->SearchResults.Num() > 0)
		{
			//Could make the Session be of Client choice
			//Maybe showcase possible sessions on the UI
			SessionInterface->JoinSession(0, TEXT("Test Session"), SessionSearch->SearchResults[0]);
		}
	}
}

//Delegate function, called after CreateSession()
//If session name already exists, bsuccess = false
void UMainGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (bSuccess)
	{
		//Move client to multiplayer map
		UWorld* World = GetWorld();
		UE_LOG(LogTemp, Warning, TEXT("Session Created Successfully"));
		UE_LOG(LogTemp, Warning, TEXT("Session Created Successfully: %s - Joining Session"), *SessionName.ToString());

		if (World)
		{
			FInputModeGameOnly InputState;
			GetFirstLocalPlayerController()->SetInputMode(InputState);
			World->ServerTravel(TEXT("/Game/Levels/MultiplayerMap?listen"));
		}
		//FindSession();
	}
	else
	{
		//If CreateSession() caused bSucess to be false
		//due to the sessionname already existing, delete that existing session
		//Then create a new one when Destroying is complete
		UE_LOG(LogTemp, Warning, TEXT("Session was not Created"));
		DestroySession(SessionName);
	}
}

void UMainGameInstance::OnDestroySessionComplete(FName SessionName, bool bSuccess)
{
	//If Session is destroyed 
	if (bSuccess && SessionInterface.IsValid())
	{
		CreateSession(SessionName);
	}
}

void UMainGameInstance::OnFindSessionComplete(bool bSuccess)
{
	if (bSuccess)
	{
		//Stores a array of possible sessions to join. 
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		UE_LOG(LogTemp, Warning, TEXT("Found %i Sessions"), SearchResults.Num())
			for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
			{
				UE_LOG(LogTemp, Warning, TEXT("Found Session: %s"), *SearchResult.GetSessionIdStr())
			}
		JoinSession();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Sessions was not successful"))
	}
}

void UMainGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult)
{
	if (SessionInterface.IsValid())
	{
		FString Address;
		SessionInterface->GetResolvedConnectString(SessionName, Address);

		APlayerController* PlayerController = GetFirstLocalPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputState;
			GetFirstLocalPlayerController()->SetInputMode(InputState);
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}