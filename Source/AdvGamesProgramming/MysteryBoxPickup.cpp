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
	
	//Goal
	//Maybe here choose what type of pickup it will be
	//Variation in shape, model, etc. 
	int32 RandomHealth = FMath::RandRange(1, 100);
	HealthAmount = RandomHealth; 

	//Speed Mulitplier for Walk Speed
	float RandomSpeedMultiplier = FMath::FRandRange(1.5, 5);
	SpeedMultiplier = RandomSpeedMultiplier;

	//Set amount of times Box has been touched to zero
	PickupTouchCount = 0; 

	MeshComponent = FindComponentByClass<UStaticMeshComponent>();
	//Here Can Generate Stats / Characteristics
	//In BluePrint can set material

	//Select Box type
	//Each type have about a 33% chance 
	//Enum not really useful here, but more easier on readability
	//Can just use OnPick to randomly assign type  

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
	//PLACE TO SET VARIABLES created in the OnGenerate() function
	//Call Pickup class's OnPickup function
	//Super::OnPickup(ActorThatPickedUp);

	//In WeaponPickup, this function sets the Player's gun stats equal to the PickUp Stats. 
	//Goals:
	//1. Random choice of Speed Boost, HP Regen or Gun 
	//If "Speed" --> Update Player's movement speed temporarily
	//If "Health" --> Update Player's Health accordingly
	//If "Gun" --> Produce a Weapon Pickup 

	if (ActorThatPickedUp->GetClass()->GetName() == FString("PlayerCharacterBlueprint_C"))
	{
		//Initilise Variables associated with Actor that picked up the MysteryBox
		PlayerThatPickedUp = Cast<APlayerCharacter>(ActorThatPickedUp);
		MovementComponent = PlayerThatPickedUp->FindComponentByClass<UCharacterMovementComponent>();
		HealthComponent = PlayerThatPickedUp->FindComponentByClass<UHealthComponent>();

		//If MysteryBox has not been touched before
		//Then proceed with it's effects
		if (PickupTouchCount < 1)
		{
			//Apply effect of selected box type
			if (Type == MysteryBoxPickupType::HEALTH)
			{
				//Health Portion
				//Cast Actor to PlayerCharacter to attain its Class Object
				if (PlayerThatPickedUp) {
					//Access Health Component
					
					if (HealthComponent && HealthMaterial)
					{
						//Change MysteryBox material to respective Health Material (Green)
						//Execute Health Recovery function
						MeshComponent->SetMaterial(0, HealthMaterial);
						HealthComponent->OnTouchHealthBoost(HealthAmount);
						UE_LOG(LogTemp, Warning, TEXT("Health Recovered by: %i"), HealthAmount);
					}
				}
			}
			else if (Type == MysteryBoxPickupType::WEAPON)
			{
				if (MeshComponent && WeaponMaterial)
				{
					//Change MysteryBox to WeaponMaterial(Blue)
					MeshComponent->SetMaterial(0, WeaponMaterial);
				}
				//Spawn Pickup in front of Player
				FVector Location = GetActorLocation();
				FVector DirectionToTarget = ActorThatPickedUp->GetActorForwardVector();
				Location += (DirectionToTarget * 150);

				//Spawn WeaponPickup 
				APickup* WeaponPickup = GetWorld()->SpawnActor<APickup>(WeaponPickupClass, Location, FRotator::ZeroRotator);
				UE_LOG(LogTemp, Warning, TEXT("WeaponPickup Spawned from MysteryBox"));
			}
			else if (Type == MysteryBoxPickupType::SPEED_BOOST)
			{
				if (PlayerThatPickedUp) {
					//Access Movement Component
					if (MovementComponent && BoostMaterial)
					{
						//Change MysteryBox material to respective Health Material (Green)
						//Execute Health Recovery function
						MeshComponent->SetMaterial(0, BoostMaterial);
						MovementComponent->MaxWalkSpeed *= SpeedMultiplier;
						UE_LOG(LogTemp, Warning, TEXT("Speed Boost by: %f"), SpeedMultiplier);

						//Remove Mysterbox - Move Box out of visible map after 1 second
						GetWorld()->GetTimerManager().SetTimer(FirstHandle, this, &AMysteryBoxPickup::MoveBoxDown, 1.0f, false);
						//Reset movement speed after 5 seconds
						GetWorld()->GetTimerManager().SetTimer(SecondHandle, this, &AMysteryBoxPickup::ResetSpeed, 3.0f, false);
					}
				}
			}
			//Destroy Object after specified time
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
		UE_LOG(LogTemp, Warning, TEXT("Movement Component is NOT NULL"));
		//Divide by Speed Multiplier to get original movement speed
		MovementComponent->MaxWalkSpeed /= SpeedMultiplier;
	}
}

void AMysteryBoxPickup::MoveBoxDown()
{
	//Grab Actor location and move down along the z axis
	FVector NewLocation = GetActorLocation();
	NewLocation.Z *= -5;
	SetActorLocation(NewLocation);
}

