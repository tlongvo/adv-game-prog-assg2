// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "GameFramework/Actor.h"
#include "ProcedurallyGeneratedMap.h"
#include "NavigationNode.h"
#include "TimerManager.h"
#include "MysteryBoxPickup.h"
#include "PickupManager.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API APickupManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupManager();
	AProcedurallyGeneratedMap* ProceduralMap;
	AMysteryBoxPickup* MysteryBox;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//From Lab
	void Init(const TArray<FVector>& SpawnLocations, TSubclassOf<class APickup> WeaponPickup, TSubclassOf<APickup> MysteryBoxPickup, float FrequencyOfSpawn);

	//Custom Functions
	int32 GetNumberOfCharacters(); //Return number of Characters in the world
	int32 GetNumberOfMysteryBoxes(); //Return number of MysteryBoxes in the world

private:
	//From Lab
	TArray<FVector> PossibleSpawnLocations;
	TSubclassOf<class APickup> WeaponPickupClass;
	float FrequencyOfWeaponPickupSpawns;
	FTimerHandle WeaponSpawnTimer;

	//Custom
	//TArray<FVector> PossibleSpawnLocations;
	TSubclassOf<APickup> MysteryBoxPickupClass;
	ANavigationNode* Node;
	FTimerHandle MysteryBoxSpawnTimer;

	//Delay in seconds between spawns of Mystery Boxes, Editable within editor
	UPROPERTY(EditAnywhere)
	float FrequencyOfMysteryBoxSpawns;
	
	
	//Functions
	void SpawnMysteryBoxPickup();
	FVector GenerateLocation(); //Generate Location of MysteryBoxPickup based on it's Type

	/** Generate an array that sorts the given array by specified order
	 *	@param NodeLocations: The array of all the node locations
	 *	@param bIsAscendingOrder: Check whether Array is sorted in Descending order; Default is Ascending
	*/
	TArray<FVector> SortNodesByZValue(TArray<FVector> NodeLocations, bool bIsDescendingOrder); 
	
	/**
	 * Will spawn a single weapon pickup in a random location from the PossibleSpawnLocation array.
	 */
	void SpawnWeaponPickup();
};
