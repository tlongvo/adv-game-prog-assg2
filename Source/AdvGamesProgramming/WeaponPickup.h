// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WeaponFiringType.h"
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnPickup(AActor* ActorThatPickedUp) override;
	UFUNCTION(BlueprintCallable)
		void OnGenerate() override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		WeaponPickupRarity Rarity;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		WeaponFiringType FiringType;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		float BulletDamage;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		float MuzzleVelocity;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		int32 MagazineSize;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		float WeaponAccuracy;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
		float FireRate;

private:

	/** Will generate an array of a given length with a certain number of true values.
	 *	@param ArrayLength: The length of the wanted array.
	 *	@param NumTrue: The number of true values that are wanted in the array.
	 *	@param RandBoolArray: The random array that will be populated with values
	*/
	void GenerateRandBooleanArray(int32 ArrayLength, int32 NumTrue, TArray<bool>& RandBoolArray);

};
