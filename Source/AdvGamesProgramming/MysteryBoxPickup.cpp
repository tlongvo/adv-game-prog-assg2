// Fill out your copyright notice in the Description page of Project Settings.


#include "MysteryBoxPickup.h"
#include "PlayerCharacter.h"
#include "WeaponPickup.h"
#include "Components/StaticMeshComponent.h"
#include "HealthComponent.h"

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
	int32 RandomInt = FMath::RandRange(1, 100);
	HealthAmount = RandomInt; 

	MeshComponent = FindComponentByClass<UStaticMeshComponent>();
	//Here Can Generate Stats / Characteristics
	//In BluePrint can set materials
}


void AMysteryBoxPickup::OnPickup(AActor* ActorThatPickedUp)
{
	//Call Pickup class's OnPickup function
	Super::OnPickup(ActorThatPickedUp);
	
	
	int32 RandomPercentageNumber = FMath::RandRange(1, 100);
	//FVector OldLocation = GetActorLocation();
	
	if (RandomPercentageNumber <= 50)
	{
		//Health Portion
		//There no Health deduction against Player only enemy
		//Cast Actor to PlayerCharacter
		APlayerCharacter* PlayerThatPickedUp = Cast<APlayerCharacter>(ActorThatPickedUp);
		if (PlayerThatPickedUp) {
			//Access Health Component
			UHealthComponent* Health = PlayerThatPickedUp->FindComponentByClass<UHealthComponent>();
			if (Health && HealthMaterial)
			{
				MeshComponent->SetMaterial(0, HealthMaterial); 
				Health->OnTouchHealthBoost(HealthAmount);
			}
		}
	}
	else if (RandomPercentageNumber <= 100)
	{
		if (MeshComponent && WeaponMaterial)
		{
			MeshComponent->SetMaterial(0, WeaponMaterial);			
		}
		//MysteryBox Pickup
		AWeaponPickup* WeaponPickup = GetWorld()->SpawnActor<AWeaponPickup>(WeaponPickupClass, this->GetActorLocation(), FRotator::ZeroRotator);
		UE_LOG(LogTemp, Warning, TEXT("WeaponPickup Spawned from MysteryBox"));
		

	}
	this->SetLifeSpan(5.0f);

	//In WeaponPickup, this function sets the Player's gun stats equal to the PickUp Stats. 
	//Goals:
	//1. Random choice of Speed Boost, HP Regen or Gun 
	//If "Speed" --> Update Player's movement speed temporarily
	//If "Health" --> Update Player's Health accordingly
	//If "Gun" --> Update Player's Gun Stats 
	//Gun -- Despawn Box and Generate Gun

	//For Now Create the Speed Boost
} 

