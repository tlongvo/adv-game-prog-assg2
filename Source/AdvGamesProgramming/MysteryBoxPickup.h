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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 HealthAmount; 
public:
	
	//Blueprint usuable is temporary
	UFUNCTION(BlueprintCallable)
		void OnGenerate() override;
	//UFUNCTION(BlueprintImplementableEvent)
		void OnPickup(AActor* ActorThatPickedUp) override;
	
	
};
