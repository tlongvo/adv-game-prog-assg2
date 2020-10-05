// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "GameFramework/Actor.h"
#include "ProcedurallyGeneratedMap.h"
#include "NavigationNode.h"
#include "TimerManager.h"
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

	TArray<FVector> PossibleSpawnLocations;
	TArray<ANavigationNode*> MapNodes; 
	ANavigationNode* Node;
	FTimerHandle MysteryBoxSpawnTimer;
	float FrequencyOfMysteryBoxSpawns; 
	int32 NumberOfMysteryBoxes;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnMysteryBoxPickup();
	int32 GetNumberOfCharacters();

private:

	void GetNodeLocations();
};
