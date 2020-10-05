// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupManager.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "WeaponPickup.h"

// Sets default values
APickupManager::APickupManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FrequencyOfMysteryBoxSpawns = 3.0f; //3 second delay between spawns 
	
}

// Called when the game starts or when spawned
void APickupManager::BeginPlay()
{
	Super::BeginPlay();
	PossibleSpawnLocations = GetNodeLocations();
	//Calls the Spawn function after a Delay (FrequencyOfMysteryBoxSpawns)
	//Function call is looped via "true" paramater
	GetWorldTimerManager().SetTimer(MysteryBoxSpawnTimer, this,
		&APickupManager::SpawnMysteryBoxPickup, FrequencyOfMysteryBoxSpawns, true, 0.0f);
}

// Called every frame
void APickupManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Function only works for Procedurally Generated Maps
//Due to it getting the Vertices Array from a Procedural Map
TArray<FVector> APickupManager::GetNodeLocations()
{
	TArray<FVector> NodeLocations; 
	for (TActorIterator<AProcedurallyGeneratedMap> It(GetWorld()); It; ++It)
	{
		ProceduralMap = *It;
		if (ProceduralMap)
		{
			NodeLocations = ProceduralMap->Vertices;
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
				NodeLocations.Add(Node->GetTargetLocation());
			}
		}
	}

	return NodeLocations;
}

void APickupManager::SpawnMysteryBoxPickup()
{
	//Check for existance of Nodes on Map
	//And limit No. of Mystery Boxes to No. of Characters 
	if (PossibleSpawnLocations.IsValidIndex(0) && GetNumberOfMysteryBoxes() < GetNumberOfCharacters())
	{
		MysteryBox = GetWorld()->SpawnActor<AMysteryBoxPickup>(MysteryBoxPickupClass, FVector::ZeroVector, FRotator::ZeroRotator);
		//Set New Location based MysteryBox Type
		FVector NewLocation = GenerateLocation();
		MysteryBox->SetActorLocation(NewLocation);
	}
}

//TEMP FUNCTION -- USE IF NEEDED
//Current Location Generation is fine. 
FVector APickupManager::GenerateLocation()
{
	//Generate Location based on the following:
	//Player Health
	//Type of Effect 
	//If weapon, Rarity of Weapon --Impossible to Get rarity, because it's assigned upon pickup

	//Generate Location if Weapon Pickup
	//Weapons Higher spot
	//Health/Speed Boost lower spectrum 
	//This bit can be pcg
	//Algorithm for finding highest node via Z
	FVector NewSpawnLocation = FVector::ZeroVector; 
	if (MysteryBox)
	{
		if (MysteryBox->Type == MysteryBoxPickupType::WEAPON)
		{
			UE_LOG(LogTemp, Warning, TEXT("WEAPON IS VALID: NOW GENERATING NEW LOCATION"));
			//Find Highest Node
			FVector HighestPoint = FVector::ZeroVector;
			float HighestZValue = TNumericLimits<float>::Min();

			for (FVector CurrentNode : PossibleSpawnLocations)
			{
				if (CurrentNode.Z > HighestZValue)
				{
					HighestZValue = CurrentNode.Z;
					HighestPoint = CurrentNode;
				}
			}

			//Z value Offset to avoid object touching spawning in ground 
			HighestPoint.Z += 100.0f;
			NewSpawnLocation = HighestPoint;


		}
		else
		{
			/*
			FVector LowestPoint = FVector::ZeroVector;
			float LowestZValue = TNumericLimits<float>::Max();

			for (FVector CurrentNode : PossibleSpawnLocations)
			{
				//If current Node Height is less than Current Lowest Z value
				if (CurrentNode.Z < LowestZValue)
				{
					LowestZValue = CurrentNode.Z;
					LowestPoint = CurrentNode;
				}
			}
			//Z value Offset to avoid object touching spawning in ground 
			LowestPoint.Z += 100.0f;
			UE_LOG(LogTemp, Warning, TEXT("EFFECTS SPAWNING AT LOWEST POINT"));
			NewSpawnLocation = LowestPoint;
			*/
			/*
			//Choose a Random location
			//Offset the Height upwards to avoid Object touching the ground
			int32 RandomIndex = FMath::RandRange(0, PossibleSpawnLocations.Num() - 1);
			FVector SpawnLocation = PossibleSpawnLocations[RandomIndex];
			SpawnLocation.Z += 100.0f;

			return SpawnLocation;
			*/
			int32 EnumCount = 0;
			UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("MysteryBoxPickupType"), true);
			if(EnumPtr)
			{
				//Minus as there's an additional Enum element added by unreal
				EnumCount = EnumPtr->NumEnums() - 1;
			}
			UE_LOG(LogTemp, Warning, TEXT("ENUM SIZE, %i"), EnumCount);

			int32 LowPoints = PossibleSpawnLocations.Num() / EnumCount;

			//The Lowest 1/3 of the Sorted Array reflects the lowest points
			//Now choose a point within that range to spawn the item
			int32 LowRangeIndex = FMath::FRandRange(0, LowPoints);
			
			//Produce a Sorted Array 
			TArray<FVector> SortedArray = SortNodesByZValue(PossibleSpawnLocations, false);
			//Assign new spawn location 
			NewSpawnLocation = SortedArray[LowRangeIndex];
			NewSpawnLocation.Z += 100.0f;
		}
	}
	
	return NewSpawnLocation; 
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

int32 APickupManager::GetNumberOfMysteryBoxes()
{
	int32 NumberOfMysteryBoxes = 0;
	for (TActorIterator<AMysteryBoxPickup> It(GetWorld()); It; ++It)
	{
		NumberOfMysteryBoxes++;
	}
	//UE_LOG(LogTemp, Warning, TEXT("No of Characters, %i"), NumberOfCharacters);
	return NumberOfMysteryBoxes;
}

TArray<FVector> APickupManager::SortNodesByZValue(TArray<FVector> NodeLocations, bool bIsDescendingOrder)
{
	//Implement Sorting Algorithm
	//Bubble Sort -- easiest but slow 
	//Quick Sort -- harder but quicker 
	TArray<FVector> SortedArray = NodeLocations;

	if (bIsDescendingOrder)
	{
		//Descending order sorting
	}
	else
	{
		//Ascending order sorting
		bool bSwappedHappened;
		do
		{
			bSwappedHappened = false;
			//Loop through each node
			//To avoid reach outside the bounds of the array
			//we minus 1 from the comparison
			for (int32 i = 0; i < PossibleSpawnLocations.Num() - 1; i++)
			{
				//If current Node is higher than the next node
				//Swap position in the array 
				if (SortedArray[i].Z > SortedArray[i + 1].Z)
				{
					Swap(SortedArray[i], SortedArray[i + 1]);
					bSwappedHappened = true; 
				}
			}
		} while (bSwappedHappened);
	}
	return SortedArray; 
}