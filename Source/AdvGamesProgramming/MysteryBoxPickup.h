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

	MysteryBoxPickupType Type; 

	//Movement Speed Variables
	UPROPERTY(EditAnywhere)
		float MaxSpeedMultiplier;
	float BaseSpeedMultiplier;
	float SpeedMultiplier;
	

	//Health
	float HealthAmount;
	UHealthComponent* HealthComponent;

	//Utility variables
	UStaticMeshComponent* MeshComponent;
	APlayerCharacter* PlayerThatPickedUp;
	UCharacterMovementComponent* MovementComponent;
	AWeaponPickup* WeaponPickup;
	
	
private:
	bool bHasBeenTouched; //Check if OnPickup() has been executed once
	FTimerHandle FirstHandle; //Handle for MoveBoxDown()
	FTimerHandle SecondHandle; //Handle for ResetSpeed() 
public:
	
	UFUNCTION(BlueprintCallable)
		void OnGenerate() override; //Generate "Type" of Effect

	//Create and execute functionality of chosen "Type"
	UFUNCTION(BlueprintCallable)
		void OnPickup(AActor* ActorThatPickedUp) override;

	UFUNCTION()
		void ResetSpeed();

	UFUNCTION()
		void MoveBoxDown();

	/** Generate a value for "HealthAmount" based on PlayerCharacter Health
	* @param PlayerHealthComponent: Health component attached to the Player
	*/
	void GenerateAndSetHealthAmount(UHealthComponent* PlayerHealthComponent);

	/** Generate a value for "SpeedMultiplier" based on PlayerCharacter Health
	* @param PlayerHealthComponent: Health component attached to the Player
	* @param PlayerMovementComponent: Character Movement component 
	*/
	void GenerateAndSetSpeedMultiplier(UHealthComponent* PlayerHealthComponent,
		UCharacterMovementComponent* PlayerMovementComponent);

};
