// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "WeaponPickup.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealthComponent.h"
#include "MysteryBoxPickup.generated.h"

UENUM()
enum class MysteryBoxPickupType : uint8
{
	HEALTH,
	SPEED_BOOST,
	WEAPON
};

UCLASS()
class ADVGAMESPROGRAMMING_API AMysteryBoxPickup : public APickup
{
	GENERATED_BODY()

public:
	AMysteryBoxPickup();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 HealthAmount; 

	UPROPERTY(VisibleAnywhere)
	float SpeedMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		MysteryBoxPickupType Type;

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
	
	APlayerCharacter* PlayerThatPickedUp;
	UCharacterMovementComponent* MovementComponent;
	UHealthComponent* HealthComponent;

	FTimerHandle FirstHandle;
	FTimerHandle SecondHandle;

private:
	int32 PickupTouchCount; 
public:
	
	//Blueprint usuable is temporary
	UFUNCTION(BlueprintCallable)
		void OnGenerate() override;
	UFUNCTION(BlueprintCallable)
		void OnPickup(AActor* ActorThatPickedUp) override;

	UFUNCTION()
	void ResetSpeed();

	UFUNCTION()
		void MoveBoxDown();
};
