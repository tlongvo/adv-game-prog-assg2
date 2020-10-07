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
	//Materials
	UPROPERTY(EditAnywhere)
		UMaterial* HealthMaterial;
	UPROPERTY(EditAnywhere)
		UMaterial* WeaponMaterial;
	UPROPERTY(EditAnywhere)
		UMaterial* BoostMaterial;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AWeaponPickup> WeaponPickupClass;

	//Movement Speed Variables
	float BaseSpeedMultiplier;
	float SpeedMultiplier;
	UPROPERTY(EditAnywhere)
		float MaxSpeedMultiplier;

	//Health
	int32 HealthAmount;
	UHealthComponent* HealthComponent;

	MysteryBoxPickupType Type;
	UStaticMeshComponent* MeshComponent;
	AWeaponPickup* WeaponPickup;
	APlayerCharacter* PlayerThatPickedUp;
	UCharacterMovementComponent* MovementComponent;
	
	
private:
	int32 PickupTouchCount; 
	FTimerHandle FirstHandle;
	FTimerHandle SecondHandle;
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

	/** Generate a value for "HealthAmount" based on PlayerCharacter Current Health
	* @param PlayerHealthComponent: Health component attached to the Player
	*/
	void GenerateHealthAmount(UHealthComponent* PlayerHealthComponent);

};
