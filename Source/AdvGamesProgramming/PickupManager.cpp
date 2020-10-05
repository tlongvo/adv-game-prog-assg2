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
	TArray<FVector> SortedNodesArray = SortNodesByZValue(PossibleSpawnLocations, false);

	//Get a count for types of effects the Mytery Box has 
	int32 NumberOfTypes = 0;
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("MysteryBoxPickupType"), true);
	if (EnumPtr)
	{
		//Minus as there's an additional Enum element added by unreal
		NumberOfTypes = EnumPtr->NumEnums() - 1;
	}
	UE_LOG(LogTemp, Warning, TEXT("Number of Mystery Box Types, %i"), NumberOfTypes);

	//Mystery Box will spawn at different heights of the map depending on its "Type"
	//Weapon - Highest 1/3(33%) of points will contain weapons
	//Boost Effects (Health, Speeed) - Lowest 1/3 of points will contain boost effects
	//Middle Range will not contain any mystery boxes.
	//Note: There are only 3 Types currently
	//----- Varying number of "Types" will cause changes to its range
	//----- These comments will refer to Mystery Boxes have only 3 types.
	if (MysteryBox)
	{
		if (MysteryBox->Type == MysteryBoxPickupType::WEAPON)
		{
			UE_LOG(LogTemp, Warning, TEXT("WEAPON IS VALID: NOW GENERATING NEW LOCATION"));
			//Choose a random point within highest points of the map (Top 33%)
			//Sort the Array asecndingly by z-value or height
			
			//Grab the Range of the High Points
			//Starting value is the number of Nodes minus 1/3 of the number of nodes
			//This will produce a number representing 66% of the Number of Nodes
			int32 HighPointsStart = SortedNodesArray.Num() - (SortedNodesArray.Num() / NumberOfTypes); 
			int32 HighPointsEnd = SortedNodesArray.Num();
			int32 HighRangeIndex = FMath::FRandRange(HighPointsStart, HighPointsEnd);
			
			//Set new spawn within the high range
			NewSpawnLocation = SortedNodesArray[HighRangeIndex];
			//Offset height upwards to avoid collision with the ground
			NewSpawnLocation.Z += 100.0f;
		}
		else
		{
			int32 LowPoints = PossibleSpawnLocations.Num() / NumberOfTypes;

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
		//Bubble Sorting Algorithm
		bool bSwappedHappened;
		//Do executed at least once then while statement check occurs
		do
		{
			bSwappedHappened = false;
			//Loop through each node
			//To avoid reach outside the bounds of the array
			//we minus 1 from the comparison since we use i++
			for (int32 i = 0; i < PossibleSpawnLocations.Num() - 1; i++)
			{
				//If current Node is lower than the next node
				//Makes the larger node come first in the Array
				//Swap position in the array 
				if (SortedArray[i].Z < SortedArray[i + 1].Z)
				{
					Swap(SortedArray[i], SortedArray[i + 1]);
					bSwappedHappened = true;
				}
			}
		} while (bSwappedHappened);
	}
	else 
	{
		//Ascending order sorting
		//Bubble Sorting Algorithm
		bool bSwappedHappened;
		do
		{
			bSwappedHappened = false;
			//Loop through each node
			//To avoid reach outside the bounds of the array
			//we minus 1 from the comparison since we use i++
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