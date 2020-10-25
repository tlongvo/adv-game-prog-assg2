// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponPickup.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Net/UnrealNetwork.h"
void AWeaponPickup::OnGenerate()
{
	APickup::OnGenerate();
	//Pick a weapon rarity tier:
	//LEGENDARY = 5% chance
	//MASTER = 15% chance
	//RARE = 30% chance
	//COMMON = 50% chance
	UE_LOG(LogTemp, Warning, TEXT("GENERATING WEAPON PICKUP"))
		int32 RandomRarityValue = FMath::RandRange(1, 100);
	int32 RandomWeaponValue = FMath::RandRange(1, 4);

	// Pick a random weapon firing type
	switch (RandomWeaponValue)
	{
	case 1:
		FiringType = WeaponFiringType::SINGLE_SHOT;
		break;

	case 2:
		FiringType = WeaponFiringType::MULTI_SHOT;
		break;

	case 3:
		FiringType = WeaponFiringType::AUTOMATIC;
		break;

	case 4:
		FiringType = WeaponFiringType::SNIPER;
		break;
	}

	// Generate bool array and set rarity based on random chance
	TArray<bool> RandBoolArray;
	if (RandomRarityValue <= 5)
	{
		Rarity = WeaponPickupRarity::LEGENDARY;
		GenerateRandBooleanArray(5, 4, RandBoolArray);
	}
	else if (RandomRarityValue <= 20)
	{
		Rarity = WeaponPickupRarity::MASTER;
		GenerateRandBooleanArray(5, 3, RandBoolArray);
	}
	else if (RandomRarityValue <= 50)
	{
		Rarity = WeaponPickupRarity::RARE;
		GenerateRandBooleanArray(5, 1, RandBoolArray);
	}
	else
	{
		Rarity = WeaponPickupRarity::COMMON;
		GenerateRandBooleanArray(5, 0, RandBoolArray);
	}

	// Set the statistics for each firing type based on custom ranges
	switch(FiringType)
	{
	case WeaponFiringType::SINGLE_SHOT:

		BulletDamage = (RandBoolArray[0] ? FMath::RandRange(15.0f, 20.0f) : FMath::RandRange(5.0f, 15.0f));
		MuzzleVelocity = (RandBoolArray[1] ? FMath::RandRange(10000.0f, 20000.0f) : FMath::RandRange(5000.0f, 10000.0f));
		MagazineSize = (RandBoolArray[2] ? FMath::RandRange(15, 25) : FMath::RandRange(5, 10));
		WeaponAccuracy = (RandBoolArray[3] ? FMath::RandRange(300.0f, 1000.0f) : FMath::RandRange(50.0f, 200.0f));
		FireRate = (RandBoolArray[4] ? FMath::RandRange(0.1f, 0.2f) : FMath::RandRange(0.2f, 0.4f));

		break;

	case WeaponFiringType::MULTI_SHOT:

		BulletDamage = (RandBoolArray[0] ? FMath::RandRange(5.0f, 15.0f) : FMath::RandRange(2.0f, 10.0f));
		MuzzleVelocity = (RandBoolArray[1] ? FMath::RandRange(10000.0f, 20000.0f) : FMath::RandRange(5000.0f, 10000.0f));
		MagazineSize = (RandBoolArray[2] ? FMath::RandRange(20, 40) : FMath::RandRange(5, 20));
		WeaponAccuracy = (RandBoolArray[3] ? FMath::RandRange(50.0f, 80.0f) : FMath::RandRange(10.0f, 30.0f));
		FireRate = (RandBoolArray[4] ? FMath::RandRange(0.2f, 0.3f) : FMath::RandRange(0.4f, 0.5f));

		break;

	case WeaponFiringType::AUTOMATIC:

		BulletDamage = (RandBoolArray[0] ? FMath::RandRange(5.0f, 10.0f) : FMath::RandRange(1.0f, 5.0f));
		MuzzleVelocity = (RandBoolArray[1] ? FMath::RandRange(15000.0f, 20000.0f) : FMath::RandRange(10000.0f, 10000.0f));
		MagazineSize = (RandBoolArray[2] ? FMath::RandRange(20, 40) : FMath::RandRange(20, 30));
		WeaponAccuracy = (RandBoolArray[3] ? FMath::RandRange(50.0f, 100.0f) : FMath::RandRange(10.0f, 60.0f));
		FireRate = (RandBoolArray[4] ? FMath::RandRange(0.08f, 0.2f) : FMath::RandRange(0.2f, 0.3f));

		break;

	case WeaponFiringType::SNIPER:

		BulletDamage = (RandBoolArray[0] ? FMath::RandRange(50.0f, 100.0f) : FMath::RandRange(30.0f, 50.0f));
		MuzzleVelocity = (RandBoolArray[1] ? FMath::RandRange(15000.0f, 20000.0f) : FMath::RandRange(10000.0f, 15000.0f));
		MagazineSize = (RandBoolArray[2] ? FMath::RandRange(5, 10) : FMath::RandRange(1, 5));
		WeaponAccuracy = (RandBoolArray[3] ? FMath::RandRange(800.0f, 1000.0f) : FMath::RandRange(500.0f, 800.0f));
		FireRate = (RandBoolArray[4] ? FMath::RandRange(0.3f, 0.5f) : FMath::RandRange(0.6f, 0.8f));

		break;
	}
}

void AWeaponPickup::GenerateRandBooleanArray(int32 ArrayLength, int32 NumTrue, TArray<bool> & RandBoolArray)
{
	for (int32 i = 0; i < ArrayLength; i++)
	{
		//Ternary Condition
		RandBoolArray.Add(i < NumTrue ? true : false);
	}

	//Card Shuffling Algorithm
	for (int32 i = 0; i < RandBoolArray.Num(); i++)
	{
		int32 RandIndex = FMath::RandRange(0, RandBoolArray.Num() - 1);
		bool Temp = RandBoolArray[i];
		RandBoolArray[i] = RandBoolArray[RandIndex];
		RandBoolArray[RandIndex] = Temp;
	}

}

void AWeaponPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponPickup, Rarity);
	DOREPLIFETIME(AWeaponPickup, FiringType);
	DOREPLIFETIME(AWeaponPickup, BulletDamage);
	DOREPLIFETIME(AWeaponPickup, MuzzleVelocity);
	DOREPLIFETIME(AWeaponPickup, MagazineSize);
	DOREPLIFETIME(AWeaponPickup, WeaponAccuracy);
	DOREPLIFETIME(AWeaponPickup, FireRate);
}
