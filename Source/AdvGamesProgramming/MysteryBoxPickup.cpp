// Fill out your copyright notice in the Description page of Project Settings.

#include "MysteryBoxPickup.h"
#include "PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h" 
#include "Engine/GameEngine.h"

AMysteryBoxPickup::AMysteryBoxPickup()
{
	//WeaponMaterial = CreateDefaultSubobject<UMaterial>(TEXT("WeaponMaterial"));
	//HealthMaterial = CreateDefaultSubobject<UMaterial>(TEXT("HealthMaterial"));
	//BoostMaterial = CreateDefaultSubobject<UMaterial>(TEXT("BoostMaterial"));

	//Initialise Speed Variables
	SpeedMultiplier = 0.0f;
	BaseSpeedMultiplier = 0.2f; //Offset to prevent Multiplier equating to 1
	MaxSpeedMultiplier = 5.0f;

	bHasBeenTouched = false;
	HealthAmount = 0;
}

void AMysteryBoxPickup::OnGenerate() //Call in Blueprint to execute
{
	Super::OnGenerate();
	//Goal: Determine the "Type" of the Mystery Box

	//Randomly select Box type (33% chance per type)
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

void AMysteryBoxPickup::OnPickup(AActor* ActorThatPickedUp) //Generates stats/effects for the chosen "Type"
{
	Super::OnPickup(ActorThatPickedUp);
	/**Description of each "Type" functionality
	* If "Speed" --> Increase Player's movement speed temporarily
	* If "Health" --> Recover Player's health
	* If "Gun" --> Produce a Weapon pickup
	*/
	//Initilise Mesh component
	//MeshComponent = FindComponentByClass<UStaticMeshComponent>();
	//Get Destructible Component
	DestructibleComponent = FindComponentByClass<UDestructibleComponent>();

	//Permit PlayerCharacter to touch box
	//Cast Actor to PlayerCharacter to attain a PlayerCharacter type pointer
	if (Cast<APlayerCharacter>(ActorThatPickedUp))
	{
		//Initilise Variables associated with Actor that picked up the MysteryBox
		PlayerThatPickedUp = Cast<APlayerCharacter>(ActorThatPickedUp); //Don't Assign outside the if statement, otherwise it can return null
		MovementComponent = PlayerThatPickedUp->FindComponentByClass<UCharacterMovementComponent>();
		HealthComponent = PlayerThatPickedUp->FindComponentByClass<UHealthComponent>();

		//If MysteryBox has not been touched
		if (!bHasBeenTouched)
		{
			//Remove Mystery Box - Move Box out of visible map after 1 second
			GetWorld()->GetTimerManager().SetTimer(FirstHandle, this, &AMysteryBoxPickup::MoveBoxDown, 1.0f, false);

			//Apply effect of selected box type
			if (Type == MysteryBoxPickupType::HEALTH)
			{
				//Access Health Component
				if (HealthComponent && HealthMaterial && DestructibleComponent)
				{
					//Change MysteryBox material to respective Health Material (Green)
					//Execute Health Recovery function
					//MeshComponent->SetMaterial(0, HealthMaterial);
					DestructibleComponent->SetMaterial(0, HealthMaterial);
					//Generate Health amount and update Player Health
					GenerateAndSetHealthAmount(HealthComponent);
				}
			}
			else if (Type == MysteryBoxPickupType::WEAPON)
			{
				if (WeaponMaterial && DestructibleComponent)
				{
					//Change MysteryBox to WeaponMaterial(M_FPGun)
					//MeshComponent->SetMaterial(0, WeaponMaterial);
					DestructibleComponent->SetMaterial(0, WeaponMaterial);
				}
				//Get Position in front of Actor (Mystery Box)
				FVector Location = GetActorLocation();
				FVector DirectionToTarget = ActorThatPickedUp->GetActorForwardVector();
				Location += (DirectionToTarget * 150);

				//Spawn WeaponPickup only if Server version touched it.
				//Prevents double spawning due to autonomous proxy
				if (PlayerThatPickedUp->HasAuthority())
				{
					WeaponPickup = GetWorld()->SpawnActor<AWeaponPickup>(WeaponPickupClass, Location, FRotator::ZeroRotator);
					UE_LOG(LogTemp, Warning, TEXT("WeaponPickup Spawned from MysteryBox"));
				}
			}
			else if (Type == MysteryBoxPickupType::SPEED_BOOST)
			{
				//Access Movement Component
				if (MovementComponent && BoostMaterial && DestructibleComponent)
				{
					//Change MysteryBox material to respective Speed Material (Yellow)
					//MeshComponent->SetMaterial(0, BoostMaterial);
					DestructibleComponent->SetMaterial(0, BoostMaterial);

					//Generate Speed Multiplier value and update Player Movement Speed
					GenerateAndSetSpeedMultiplier();
				}
			}
			//Destroy Object after specified time
			//Needs to be higher than Timer, otherwise Timer won't work (access errors)
			this->SetLifeSpan(3.1f);
			bHasBeenTouched = true;
		}
	}
} 

void AMysteryBoxPickup::ResetSpeed()
{
	//Return to Player's original movement speed

	if (PlayerThatPickedUp)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Reset Movement Speed")));
		}
		Cast<APlayerCharacter>(MovementComponent->GetOwner())->SprintEnd();
	}

}

void AMysteryBoxPickup::MoveBoxDown()
{
	//Moves Box under the map
	//Grab Box location and move down along the z axis
	FVector NewLocation = GetActorLocation();
	NewLocation.Z -= 1000.0f; //Arbitrary value 
	SetActorLocation(NewLocation);
}

void AMysteryBoxPickup::GenerateAndSetHealthAmount_Implementation(UHealthComponent* PlayerHealthComponent)
{
	/** Heal for a percentage of missing hp
	* @var MissingHealthPercentage: Percentage of missing Health
	* @var PerlinNum: Random Number using 1D Perlin Noise with Missing Health as Input
	* @var HealPercentage: Percentage of Missing Health to be healed; Affected by 
	* the MissingHealthPercentage and PerlinNum.
	*
	*/
	float HealthPercentageRemaining = PlayerHealthComponent->HealthPercentageRemaining();
	float MissingHealthPercentage = 1 - HealthPercentageRemaining;
	float MissingHealth = PlayerHealthComponent->MaxHealth * MissingHealthPercentage;

	//Absolute value to establish range between 0 to 1
	float PerlinNum = FMath::Abs(FMath::PerlinNoise1D(MissingHealth));

	//Ensure Player never Heals full amount via RandomOffset  (MissingHealth^0 = 1) 
	if (PerlinNum == 0) { PerlinNum = FMath::Abs<float>(FMath::PerlinNoise1D(MissingHealth)); }

	//Heal percentage relative to percentage of missing hp with random offset via PerlinNoise
	//"3" is a arbitrary number to lower PerlinNum value
	float Offset = MissingHealthPercentage * (PerlinNum / 3);
	float HealPercentage = MissingHealthPercentage - Offset;
	//Amount of Health to be recovered
	HealthAmount = MissingHealth * HealPercentage;

	//Update Health
	HealthComponent->OnTouchHealthBoost(HealthAmount);
	//UE_LOG(LogTemp, Warning, TEXT("MISSING HEALTH PERCENTAGE: %f"), MissingHealthPercentage);
	//UE_LOG(LogTemp, Warning, TEXT("MISSING HEALTH : %f"), MissingHealth);
	//UE_LOG(LogTemp, Warning, TEXT("HEAL PERCENTAGE: %f"), HealPercentage);
	//UE_LOG(LogTemp, Warning, TEXT("PERLIN AMOUNT by: %f"), PerlinNum);
	//UE_LOG(LogTemp, Warning, TEXT("Health Recovered by: %f"), HealthAmount);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Health Recovered by: %f"), HealthAmount));
	}
}

void AMysteryBoxPickup::GenerateAndSetSpeedMultiplier()
{
	/**Exponentially increase Movement Speed based on Missing Health
	* BaseSpeedMultiplier: Offset Value to prevent SpeedMultiplier equating to 1.
	* HealthPercentageMissing: Percentage of Player's missing Health
	*/
	float HealthPercentageMissing = 1 - HealthComponent->HealthPercentageRemaining();
	SpeedMultiplier = BaseSpeedMultiplier + FMath::Pow(MaxSpeedMultiplier, HealthPercentageMissing);
	
	/** Increase movement for locally controlled player
	* Need to consider listen server host (Player that hosts)
	* Check for Authority that is controlled
	* Check for Autonomous Proxy
	*/
	if (PlayerThatPickedUp->GetLocalRole() == ROLE_Authority && PlayerThatPickedUp->IsLocallyControlled())
	{
		PlayerThatPickedUp->IncreaseSpeed(SpeedMultiplier);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Speed Boost for Authority: %f"), SpeedMultiplier));
		}
		//UE_LOG(LogTemp, Warning, TEXT("PLAYER that touched box is AUTHORITY "));
	}
	else if (PlayerThatPickedUp->GetLocalRole() == ROLE_AutonomousProxy)
	{
		PlayerThatPickedUp->IncreaseSpeed(SpeedMultiplier);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Speed Boost for Autonomous: %f"), SpeedMultiplier));
		}
		//UE_LOG(LogTemp, Warning, TEXT("PLAYER that touched box is REMOTE "));
	}

	//Reset movement speed after 3 seconds
	GetWorld()->GetTimerManager().SetTimer(SecondHandle, this, &AMysteryBoxPickup::ResetSpeed, 3.0f, false);
}

void AMysteryBoxPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMysteryBoxPickup, Type);
}
