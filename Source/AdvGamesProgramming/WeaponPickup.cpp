// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/KismetArrayLibrary.h"
#include "WeaponPickup.h"

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
	//Will populate the RandBoolArray with a shuffled set of boolean values depending on the weapon rarity.
	TArray<bool> RandBoolArray;
	if (RandomRarityValue <= 5)
	{
		Rarity = WeaponPickupRarity::LEGENDARY;
		GenerateRandBooleanArray(4, 4, RandBoolArray);
	}
	else if (RandomRarityValue <= 20)
	{
		Rarity = WeaponPickupRarity::MASTER;
		GenerateRandBooleanArray(4, 3, RandBoolArray);
	}
	else if (RandomRarityValue <= 50)
	{
		Rarity = WeaponPickupRarity::RARE;
		GenerateRandBooleanArray(4, 1, RandBoolArray);
	}
	else
	{
		Rarity = WeaponPickupRarity::COMMON;
		GenerateRandBooleanArray(4, 0, RandBoolArray);
	}

	//Assign the good or bad weapon characteristics based on the result of the random boolean array.
	BulletDamage = (RandBoolArray[0] ? FMath::RandRange(15.0f, 30.0f) : FMath::RandRange(2.0f, 15.0f));
	MuzzleVelocity = (RandBoolArray[1] ? FMath::RandRange(10000.0f, 20000.0f) : FMath::RandRange(5000.0f, 10000.0f));
	MagazineSize = (RandBoolArray[2] ? FMath::RandRange(20, 100) : FMath::RandRange(1, 20));
	WeaponAccuracy = (RandBoolArray[3] ? FMath::RandRange(100.0f, 1000.0f) : FMath::RandRange(20.0f, 80.0f));
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


