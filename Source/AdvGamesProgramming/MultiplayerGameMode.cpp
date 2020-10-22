// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"
#include "EngineUtils.h"
#include "Engine/GameEngine.h"
#include "GameFramework/HUD.h"
#include "PlayerCharacter.h"
#include "TimerManager.h"
#include "PlayerHUD.h"

void AMultiplayerGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessages)
{
	Super::InitGame(MapName, Options, ErrorMessages);

	//Find the procedurally generated map in the world
	for (TActorIterator<AProcedurallyGeneratedMap> It(GetWorld()); It; ++It)
	{
		ProceduralMap = *It;
	}

	//Spawn the pickup manager on the server
	PickupManager = GetWorld()->SpawnActor<APickupManager>();

	//Initialise the pickup manager variables
	if (PickupManager && ProceduralMap)
	{
		PickupManager->Init(ProceduralMap->Vertices, WeaponPickupClass, MysteryBoxPickupClass, 10.0f);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to find the procedural map or unable to spawn the pickup manager"));
	}
}

void AMultiplayerGameMode::Respawn(AController* Controller)
{
	if (Controller)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Respawning")));
		}
		
		//Hide the player hud as soon as the player dies.
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Controller->GetPawn()))
		{
			PlayerCharacter->HidePlayerHUD(true);
		}

		//Remove Dead Pawn. 
		Controller->GetPawn()->SetLifeSpan(0.1f);

		//Set the respawn timer.
		FTimerHandle RespawnTimer;
		FTimerDelegate RespawnDelegate;
		//Call TriggerRespawn()
		//Which will deal with the actual respawning
		RespawnDelegate.BindUFunction(this, TEXT("TriggerRespawn"), Controller);
		GetWorldTimerManager().SetTimer(RespawnTimer, RespawnDelegate, 5.0f, false);
		
	}
}

void AMultiplayerGameMode::TriggerRespawn(AController* Controller)
{
	if (Controller)
	{
		//Create the new player pawn
		AActor* SpawnPoint = ChoosePlayerStart(Controller);
		if (SpawnPoint)
		{
			//Spawn and take possession of the new player
			APawn* NewPlayerPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation());
			if (NewPlayerPawn)
			{
				Controller->Possess(NewPlayerPawn);
				if (APlayerCharacter* Character = Cast<APlayerCharacter>(NewPlayerPawn))
				{
					UE_LOG(LogTemp, Display, TEXT("Showing the HUD"));
					Character->HidePlayerHUD(false);
				}
			}
		}
	}
}