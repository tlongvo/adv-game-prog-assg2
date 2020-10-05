// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupManager.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "MysteryBoxPickup.h"

// Sets default values
APickupManager::APickupManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FrequencyOfMysteryBoxSpawns = 3.0f; //3 second delay between spawns 
	NumberOfMysteryBoxes = 0; 
	
}

// Called when the game starts or when spawned
void APickupManager::BeginPlay()
{
	Super::BeginPlay();
	GetNodeLocations();

	//Looping Spawner
	//Calls the Spawn function after a Delay (FrequencyOfMysteryBoxSpawns)
	GetWorldTimerManager().SetTimer(MysteryBoxSpawnTimer, this,
		&APickupManager::SpawnMysteryBoxPickup, FrequencyOfMysteryBoxSpawns, true, 0.0f);
	//SpawnMysteryBoxPickup();
}

// Called every frame
void APickupManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Function only works for Procedurally Generated Maps
//Due to it getting the Vertices Array from a Procedural Map
void APickupManager::GetNodeLocations()
{
	for (TActorIterator<AProcedurallyGeneratedMap> It(GetWorld()); It; ++It)
	{
		ProceduralMap = *It;
		if (ProceduralMap)
		{
			PossibleSpawnLocations = ProceduralMap->Vertices;
		}
	}

	//If procedural map not found
	if(ProceduralMap == nullptr) 
	{
		for (TActorIterator<ANavigationNode> Itr(GetWorld()); Itr; ++Itr)
		{
			//Pointer to NavigationNode pointer
			//Assign NavigationNode to Node variable
			Node = *Itr;
			if (Node)
			{
				PossibleSpawnLocations.Add(Node->GetTargetLocation());
			}
		}
	}
}

void APickupManager::SpawnMysteryBoxPickup()
{
	//Check if there are Nodes in the Map
	//And if there are less Mystery boxes than number of Characters on the Map.
	if (PossibleSpawnLocations.Num() >= 1 && NumberOfMysteryBoxes <= GetNumberOfCharacters())
	{
		//Choose a Random location
		//Offset the Height upwards to avoid Object touching the ground
		int32 RandomIndex = FMath::RandRange(0, PossibleSpawnLocations.Num() - 1);
		FVector NewLocation = PossibleSpawnLocations[RandomIndex];
		NewLocation.Z += 100.0f; 

		GetWorld()->SpawnActor<AMysteryBoxPickup>(MysteryBoxPickupClass, NewLocation, FRotator::ZeroRotator);
		
		//Increment Box count
		NumberOfMysteryBoxes++;
	}
	
}

int32 APickupManager::GetNumberOfCharacters()
{
	int32 NumberOfCharacters = 0; 
	for (TActorIterator<ACharacter> It(GetWorld()); It; ++It)
	{
		NumberOfCharacters++;
	}
	//UE_LOG(LogTemp, Warning, TEXT("No of Characters, %i"), NumberOfCharacters);
	return NumberOfCharacters; 
}
