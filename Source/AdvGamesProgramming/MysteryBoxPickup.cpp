// Fill out your copyright notice in the Description page of Project Settings.


#include "MysteryBoxPickup.h"
#include "PlayerCharacter.h"
#include "HealthComponent.h"

void AMysteryBoxPickup::OnGenerate()
{
	Super::OnGenerate();
	//Remember to call function in BluePrint 
	int32 RandomInt = FMath::RandRange(1, 100);
	HealthAmount = RandomInt; 

	//Here Can Generate Stats / Characteristics
	//In BluePrint can set materials
}


void AMysteryBoxPickup::OnPickup(AActor* ActorThatPickedUp)
{
	//Call Pickup class's OnPickup function
	//Super::OnPickup(ActorThatPickedUp);

	//Health Portion
	//There no Health deduction against Player only enemy
	//Cast Actor to PlayerCharacter
	APlayerCharacter* PlayerThatPickedUp = Cast<APlayerCharacter>(ActorThatPickedUp);
	//Access Health Component
	UHealthComponent* Health = PlayerThatPickedUp->FindComponentByClass<UHealthComponent>(); 
	Health->OnTouchHealthBoost(HealthAmount);
	this->Destroy();
	
	

	//In WeaponPickup, this function sets the Player's gun stats equal to the PickUp Stats. 
	//Goals:
	//1. Random choice of Speed Boost, HP Regen or Gun 
	//If "Speed" --> Update Player's movement speed temporarily
	//If "Health" --> Update Player's Health accordingly
	//If "Gun" --> Update Player's Gun Stats 
	//Gun -- Despawn Box and Generate Gun

	//For Now Create the Speed Boost
} 

