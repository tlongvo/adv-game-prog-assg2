// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupManager.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "WeaponPickup.h"
#include "Engine/GameEngine.h"

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

	//Get all node locations as possible spawn locations
	//PossibleSpawnLocations = GetNodeLocations();

	//Looping function call with delay of "FrequencyOfMysteryBoxSpawns"
	//Function call is looped via "true" paramater
	GetWorldTimerManager().SetTimer(MysteryBoxSpawnTimer, this,
		&APickupManager::SpawnMysteryBoxPickup, FrequencyOfMysteryBoxSpawns, true, 0.0f);

	//Spawn WeaponPickup every "FrequencyOfWeaponPickupSpawns" seconds
	GetWorldTimerManager().SetTimer(WeaponSpawnTimer, this, &APickupManager::SpawnWeaponPickup, FrequencyOfWeaponPickupSpawns, true, 0.0f);
}

// Called every frame
void APickupManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupManager::Init(const TArray<FVector>& SpawnLocations, TSubclassOf<APickup> WeaponPickup, TSubclassOf<APickup> MysteryBoxPickup, float FrequencyOfSpawn)
{
	PossibleSpawnLocations = SpawnLocations;
	WeaponPickupClass = WeaponPickup;
	FrequencyOfWeaponPickupSpawns = FrequencyOfSpawn;
	MysteryBoxPickupClass = MysteryBoxPickup;
}

void APickupManager::SpawnWeaponPickup()
{
	//Find a random index in the array of spawn locations.
	int32 RandomIndex = FMath::RandRange(0, PossibleSpawnLocations.Num() - 1);
	//UE_LOG(LogTemp, Warning, TEXT("Node locations found %i"), PossibleSpawnLocations.Num());
	//Attempt to spawn in the weapon pickup and write a warning to the log if it was unable to spawn it in.
	if (APickup* WeaponPickup = GetWorld()->SpawnActor<APickup>(WeaponPickupClass,
		PossibleSpawnLocations[RandomIndex] + FVector(0.0f, 0.0f, 50.0f), FRotator::ZeroRotator))
	{
		WeaponPickup->SetLifeSpan(20.0f);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("Pickup Spawned")));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to spawn weapon pickup."));
	}
}

void APickupManager::SpawnMysteryBoxPickup()
{	
	//Check for existance of Nodes on map
	//And limit no. of Mystery Boxes to no. of Characters 
	if (PossibleSpawnLocations.Num() > 0 && (GetNumberOfMysteryBoxes() < GetNumberOfCharacters()))
	{
		//Spawn MysteryBox into the map with zero location and position
		MysteryBox = GetWorld()->SpawnActor<AMysteryBoxPickup>(MysteryBoxPickupClass, FVector::ZeroVector, FRotator::ZeroRotator);
		
		//Set location based on the box "Type"
		MysteryBox->SetActorLocation(GenerateLocation());
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("MysteryBox Pickup Spawned")));
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to Spawn MysteryBox "));
	}
	
}

FVector APickupManager::GenerateLocation()
{
	/* Mystery Box will spawn at different height regions of the map depending on its "Type"
	*	These regions are proportional to the number of types
	*	The current Mystery Box has only 3 "Types" 
	*	Health Recovery - Highest 1/3(33%) of points could contain Health items
	*	Weapon and Speed Boost - Lowest 1/3 of points will contain boost effects
	*	Middle Range will not contain any mystery boxes.
	*	Note: There are only 3 Types currently
	*	----- Varying number of "Types" will cause changes to its range
	*	----- These comments will refer to Mystery Boxes have only 3 types.
	*/

	//Algorithm for determining the highest region using the node's Z-Value
	FVector NewSpawnLocation = FVector::ZeroVector; 
	//Sorted Array by node height value in ascending order 
	TArray<FVector> SortedNodesArray = SortNodesByZValue(PossibleSpawnLocations, false);

	//Get a count for types of effects the Mystery Box has 
	int32 NumberOfTypes = 0;
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("MysteryBoxPickupType"), true);
	if (EnumPtr)
	{
		//Minus as there's an additional Enum element added by unreal
		NumberOfTypes = EnumPtr->NumEnums() - 1;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Number of Mystery Box Types, %i"), NumberOfTypes);

	if (MysteryBox)
	{
		if (MysteryBox->Type == MysteryBoxPickupType::HEALTH)
		{
			//UE_LOG(LogTemp, Warning, TEXT("WEAPON IS VALID: NOW GENERATING NEW LOCATION"));
			//Choose a random point within highest points of the map (Top 33%)
			
			//Grab the Range of the High Points
			//Starting value is the number of Nodes minus 1/3 of the number of nodes
			//This will produce a number representing the 66th percentile of the Number of Nodes
			int32 HighPointsStart = SortedNodesArray.Num() - (SortedNodesArray.Num() / NumberOfTypes); 
			int32 HighPointsEnd = SortedNodesArray.Num(); //End of Array aka quantity of nodes
			//Choose random node within the High region
			int32 HighRangeIndex = FMath::FRandRange(HighPointsStart, HighPointsEnd); 
			
			//Set new spawn within the high range
			NewSpawnLocation = SortedNodesArray[HighRangeIndex];
			//Offset height upwards to avoid collision with the ground
			NewSpawnLocation.Z += 100.0f;
		}
		else
		{
			//The Lower 1/3 of the Sorted Array reflects the lowest nodes in the map 
			int32 LowPointsEnd = PossibleSpawnLocations.Num() / NumberOfTypes;
			int32 LowRangeIndex = FMath::FRandRange(0, LowPointsEnd); //Randomly choose node 
			
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
	UE_LOG(LogTemp, Warning, TEXT("No of Characters, %i"), NumberOfCharacters);
	return NumberOfCharacters;
}

int32 APickupManager::GetNumberOfMysteryBoxes()
{
	int32 NumberOfMysteryBoxes = 0;
	for (TActorIterator<AMysteryBoxPickup> It(GetWorld()); It; ++It)
	{
		NumberOfMysteryBoxes++;
	}
	UE_LOG(LogTemp, Warning, TEXT("No of Boxes, %i"), NumberOfMysteryBoxes);
	return NumberOfMysteryBoxes;
}

TArray<FVector> APickupManager::SortNodesByZValue(TArray<FVector> NodeLocations, bool bIsDescendingOrder)
{
	//Sorting Algorithm - Bubble Sort
	//Sort Nodes Array by height(Z-value) ascendingly or descendingly 

	TArray<FVector> SortedArray = NodeLocations; //Get all nodes 

	if (bIsDescendingOrder)
	{
		//Descending order 
		bool bSwappedHappened;
		//Do executed at least once then while statement check occurs
		do
		{
			bSwappedHappened = false;
			//Loop through each node
			//To avoid reaching outside the bounds of the array
			//we minus 1 from the comparison since we use i++
			for (int32 i = 0; i < PossibleSpawnLocations.Num() - 1; i++)
			{
				//If current Node is lower than the next node
				//Swap position in the array 
				if (SortedArray[i].Z < SortedArray[i + 1].Z)
				{
					Swap(SortedArray[i], SortedArray[i + 1]);
					bSwappedHappened = true;
				}
			}
		} while (bSwappedHappened); //Repeat until swaps cannot occur anymore
	}
	else 
	{
		//Ascending order sorting
		bool bSwappedHappened;
		do
		{
			bSwappedHappened = false;
			for (int32 i = 0; i < PossibleSpawnLocations.Num() - 1; i++)
			{
				//If current Node is higher than the next node
				//Swap positions in the array 
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