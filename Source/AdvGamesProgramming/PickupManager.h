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

	UPROPERTY(EditAnywhere)
	TSubclassOf<APickup> MysteryBoxPickupClass; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

private:
	//Variables
	TArray<FVector> PossibleSpawnLocations;
	int32 NumberOfNodes; 
	ANavigationNode* Node;
	FTimerHandle MysteryBoxSpawnTimer;
	float FrequencyOfMysteryBoxSpawns;
	AMysteryBoxPickup* MysteryBox; 
	
	//Functions
	TArray<FVector> GetNodeLocations(); //Initalise 
	void SpawnMysteryBoxPickup();

	/** Generate an array that sorts the given array by specified order
	 *	@param NodeLocations: The array of all the node locations
	 *	@param bIsAscendingOrder: Check whether Array is sorted in Descending order; Default is Ascending
	*/
	TArray<FVector> SortNodesByZValue(TArray<FVector> NodeLocations, bool bIsDescendingOrder); 
	FVector GenerateLocation(); //Generate Location of MysteryBoxPickup based on it's Type
	int32 GetNumberOfCharacters(); //Return number of Characters in the world
	int32 GetNumberOfMysteryBoxes(); //Return number of MysteryBoxes in the world

};
