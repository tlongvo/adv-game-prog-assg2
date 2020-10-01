// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "WeaponPickup.generated.h"

UENUM(BlueprintType)
enum class WeaponPickupRarity : uint8
{
	LEGENDARY,
	MASTER,
	RARE,
	COMMON
};

/**
 *
 */
UCLASS()
class ADVGAMESPROGRAMMING_API AWeaponPickup : public APickup
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
		void OnPickup(AActor* ActorThatPickedUp) override;
	UFUNCTION(BlueprintCallable)
		void OnGenerate() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		WeaponPickupRarity Rarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float BulletDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float MuzzleVelocity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int32 MagazineSize;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float WeaponAccuracy;

private:

	/** Will generate an array of a given length with a certain number of true values.
	 *	@param ArrayLength: The length of the wanted array.
	 *	@param NumTrue: The number of true values that are wanted in the array.
	 *	@param RandBoolArray: The random array that will be populated with values
	*/
	void GenerateRandBooleanArray(int32 ArrayLength, int32 NumTrue, TArray<bool>& RandBoolArray);

};
