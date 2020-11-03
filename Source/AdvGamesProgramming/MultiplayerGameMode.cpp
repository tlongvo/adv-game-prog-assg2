// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"
#include "EngineUtils.h"
#include "Engine/GameEngine.h"
#include "GameFramework/HUD.h"
#include "PlayerCharacter.h"
#include "TimerManager.h"
#include "PlayerHUD.h"
#include "NavigationNode.h"

void AMultiplayerGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessages)
{
	Super::InitGame(MapName, Options, ErrorMessages);
	//Spawn the AI Manager on the server
	AIManager = GetWorld()->SpawnActor<AAIManager>();
	if (AIManager)
	{
		//Initialise AIManager variables
		AIManager->AgentToSpawn = EnemyCharacterClass;
		AIManager->NumAI = NumberOfAI; 
	}

	//Spawn the pickup manager on the server
	PickupManager = GetWorld()->SpawnActor<APickupManager>();

	//GetNodeLocations() finds and assigns a procedural Map to the Procedural Map variable
	//But also returns the Node Locations in the Map.
	TArray<FVector> NodeLocations = GetNodeLocations();

	//Initialise the Pickup Manager variables
	if (PickupManager && ProceduralMap)
	{
		PickupManager->Init(NodeLocations, WeaponPickupClass, MysteryBoxPickupClass, DestructibleActorClass, 10.0f);
		ProceduralMap->AIManager = AIManager; 

		//Spawn AI after a delay 
		//Helps other components load first 
		FTimerHandle SpawnTimer;
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, AIManager, &AAIManager::CreateAgents, 3.0f, false);
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
					Character->UpdateDeathHUD();
					Character->HidePlayerHUD(false);
				}
			}
		}
	}
}

TArray<FVector> AMultiplayerGameMode::GetNodeLocations() //Return all nodes locations in the map
{
	TArray<FVector> NodeLocations;

	//Getting node locations n the Procedural Map
	for (TActorIterator<AProcedurallyGeneratedMap> It(GetWorld()); It; ++It)
	{
		ProceduralMap = *It;
		if (ProceduralMap)
		{
			NodeLocations = ProceduralMap->Vertices;
		}
	}

	//If map isn't a procedural map (User created)
	if (ProceduralMap == nullptr)
	{
		ANavigationNode* Node;
		for (TActorIterator<ANavigationNode> Itr(GetWorld()); Itr; ++Itr)
		{
			//Assign the pointer to the NavigationNode pointer to Node 
			Node = *Itr;
			if (Node)
			{
				//Add to the NodeLocations Array
				NodeLocations.Add(Node->GetActorLocation());
			}
		}
	}
	return NodeLocations;
}