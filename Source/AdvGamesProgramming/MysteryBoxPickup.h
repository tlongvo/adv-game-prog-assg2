// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "MysteryBoxPickup.generated.h"

/**
 * 
 */
UCLASS()
class ADVGAMESPROGRAMMING_API AMysteryBoxPickup : public APickup
{
	GENERATED_BODY()

public:
	AMysteryBoxPickup();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 HealthAmount; 

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	UMaterial* HealthMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* WeaponMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* BoostMaterial;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APickup> WeaponPickupClass;

public:
	
	//Blueprint usuable is temporary
	UFUNCTION(BlueprintCallable)
		void OnGenerate() override;
	UFUNCTION(BlueprintCallable)
		void OnPickup(AActor* ActorThatPickedUp) override;
	
	
};
