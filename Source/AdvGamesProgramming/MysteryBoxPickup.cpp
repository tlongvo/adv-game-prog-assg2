// Fill out your copyright notice in the Description page of Project Settings.

#include "MysteryBoxPickup.h"
#include "PlayerCharacter.h"
#include "Pickup.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h" 


AMysteryBoxPickup::AMysteryBoxPickup()
{
	//WeaponMaterial = CreateDefaultSubobject<UMaterial>(TEXT("WeaponMaterial"));
	//HealthMaterial = CreateDefaultSubobject<UMaterial>(TEXT("HealthMaterial"));
}

void AMysteryBoxPickup::OnGenerate() //Remember to call function in Blueprint
{
	Super::OnGenerate();
	
	//Goal: Determine the "Type" of the Mystery Box

	//Initialise Health stats
	//int32 RandomHealth = FMath::RandRange(1, 100);
	//HealthAmount = RandomHealth; 
	//HealEffectiveness = 0.5; //Recover 50% of missing hp  


	//Set Default MaxSpeedMultiplier
	BaseSpeedMultiplier = 0.2f; //Offset to prevent Multplier equating to 1
	MaxSpeedMultiplier = 5.0f; 

	//Set amount of times Box has been touched to zero
	PickupTouchCount = 0; 

	//Initilise Mesh component
	MeshComponent = FindComponentByClass<UStaticMeshComponent>();

	//Randomly select Box type
	//Each type have about a 33% chance 
	int32 RandomPercentageNumber = FMath::RandRange(1, 100);
	if (RandomPercentageNumber <= 33)
	{
		Type = MysteryBoxPickupType::HEALTH;
	}
	else if (RandomPercentageNumber <= 66)
	{
		Type = MysteryBoxPickupType::SPEED_BOOST;
	}
	else if (RandomPercentageNumber <= 100)
	{
		Type = MysteryBoxPickupType::WEAPON;
	}
}

void AMysteryBoxPickup::OnPickup(AActor* ActorThatPickedUp)
{
	//Generates stats/effects for the chosen "Type"

	//Call Pickup class's OnPickup function
	Super::OnPickup(ActorThatPickedUp);

	//Description of each Type effect
	//If "Speed" --> Update Player's movement speed temporarily
	//If "Health" --> Update Player's Health 
	//If "Gun" --> Produce a Weapon Pickup 
	if (ActorThatPickedUp->GetClass()->GetName() == FString("PlayerCharacterBlueprint_C"))
	{
		//Initilise Variables associated with Actor that picked up the MysteryBox
		//Cast Actor to PlayerCharacter to attain its Class Object
		PlayerThatPickedUp = Cast<APlayerCharacter>(ActorThatPickedUp);
		MovementComponent = PlayerThatPickedUp->FindComponentByClass<UCharacterMovementComponent>();
		HealthComponent = PlayerThatPickedUp->FindComponentByClass<UHealthComponent>();

		//If MysteryBox has not been touched before then proceed
		if (PickupTouchCount < 1)
		{
			//Apply effect of selected box type
			if (Type == MysteryBoxPickupType::HEALTH)
			{
				if (PlayerThatPickedUp) {
					//Access Health Component
					if (HealthComponent && HealthMaterial)
					{
						//Change MysteryBox material to respective Health Material (Green)
						//Execute Health Recovery function
						MeshComponent->SetMaterial(0, HealthMaterial);
						//Generate Health amount
						GenerateHealthAmount(HealthComponent);
						HealthComponent->OnTouchHealthBoost(HealthAmount);
						UE_LOG(LogTemp, Warning, TEXT("Health Recovered by: %i"), HealthAmount);
					}
				}
			}
			else if (Type == MysteryBoxPickupType::WEAPON)
			{
				if (MeshComponent && WeaponMaterial)
				{
					//Change MysteryBox to WeaponMaterial(M_FPGun)
					MeshComponent->SetMaterial(0, WeaponMaterial);
				}
				//Get Position in front of Actor (Mystery Box)
				FVector Location = GetActorLocation();
				FVector DirectionToTarget = ActorThatPickedUp->GetActorForwardVector();
				Location += (DirectionToTarget * 150);

				//Spawn WeaponPickup 
				WeaponPickup = GetWorld()->SpawnActor<AWeaponPickup>(WeaponPickupClass, Location, FRotator::ZeroRotator);
				UE_LOG(LogTemp, Warning, TEXT("WeaponPickup Spawned from MysteryBox"));
			}
			else if (Type == MysteryBoxPickupType::SPEED_BOOST)
			{
				if (PlayerThatPickedUp) {
					//Access Movement Component
					if (MovementComponent && BoostMaterial)
					{
						//Change MysteryBox material to respective Speed Material (Yellow)
						MeshComponent->SetMaterial(0, BoostMaterial);

						//Update Speed based on Health
						float HealthPercentageMissing = 1 - HealthComponent->HealthPercentageRemaining();
						//Exponentially increase Movement Speed based on Missing Health
						//Base Multiplier to prevent SpeedMultiplier equating to 1
						SpeedMultiplier = BaseSpeedMultiplier + FMath::Pow(MaxSpeedMultiplier, HealthPercentageMissing);
						MovementComponent->MaxWalkSpeed *= SpeedMultiplier;
						UE_LOG(LogTemp, Warning, TEXT("Speed Boost by: %f"), SpeedMultiplier);

						//Remove Mystery Box - Move Box out of visible map after 1 second
						GetWorld()->GetTimerManager().SetTimer(FirstHandle, this, &AMysteryBoxPickup::MoveBoxDown, 1.0f, false);
						//Reset movement speed after 3 seconds
						GetWorld()->GetTimerManager().SetTimer(SecondHandle, this, &AMysteryBoxPickup::ResetSpeed, 3.0f, false);
					}
				}
			}
			//Destroy Object after specified time
			//Needs to be higher than Timer, otherwise Timer won't work (access errors)
			this->SetLifeSpan(3.1f);
		}
		//Increase touch count after the check has been executed. 
		PickupTouchCount++;
	}
} 

void AMysteryBoxPickup::ResetSpeed()
{
	if (MovementComponent)
	{
		//Divide by Speed Multiplier to get original movement speed
		MovementComponent->MaxWalkSpeed /= SpeedMultiplier;
		//UE_LOG(LogTemp, Warning, TEXT("Movement Component is NOT NULL"));
	}
}

void AMysteryBoxPickup::MoveBoxDown()
{
	//Moves Box out of the Map
	//Grab Box location and move down along the z axis
	FVector NewLocation = GetActorLocation();
	NewLocation.Z *= -5;
	SetActorLocation(NewLocation);
}

void AMysteryBoxPickup::GenerateHealthAmount(UHealthComponent* PlayerHealthComponent)
{
	float HealthPercentageRemaining = PlayerHealthComponent->HealthPercentageRemaining();
	//Heal for a percentage of missing hp
	//Pecentage is relative to missing hp 
	//Offset by randomness
	float MissingHealth = PlayerHealthComponent->MaxHealth * (1 - HealthPercentageRemaining);
	float MissingHealthPercentage = 1 - HealthPercentageRemaining; 

	//Absolute value to establish range between 0 to 1
	float PerlinNum = FMath::Abs(FMath::PerlinNoise1D(MissingHealth));

	//Ensure Player never Heals full amount via RandomOffset
	if (PerlinNum == 0) { PerlinNum = FMath::Abs(FMath::PerlinNoise1D(MissingHealth)); }
	//3 - Arbitrary number to lower PerlinNum
	float Offset = MissingHealthPercentage - (PerlinNum / 3); //Produces Health Percentage 
	//Amount of Health to be recovered
	HealthAmount = MissingHealth * Offset;
}
