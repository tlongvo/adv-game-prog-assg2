// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "EnemyCharacter.h"
#include "Engine/GameEngine.h"
#include "PlayerCharacter.h"
#include "PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
	MaxHealth = 100.0f;
	
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//Health Regen
	CurrentHealth += 0.0025;

	//Limit Current HP to Max HP 
	if (CurrentHealth > MaxHealth)
	{
		CurrentHealth = MaxHealth; 
	}
	/*
	if (GEngine && GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Current Health: %f"), CurrentHealth));
	}
	*/
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Replicated CurrentHealth variable
	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

void UHealthComponent::OnTakeDamage(float Damage)
{
	//Calls in listen servers where Player is also a host (server)
	if (GetOwnerRole() == ROLE_Authority)
	{
		UpdateHealthBar();
	}
	
	CurrentHealth -= Damage;
	if (CurrentHealth <= 0) 
	{
		CurrentHealth = 0;
		OnDeath();
	}

	
}

void UHealthComponent::OnDeath()
{
	APlayerCharacter* OwningPlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (OwningPlayerCharacter)
	{
		OwningPlayerCharacter->OnDeath();
	}
}

float UHealthComponent::HealthPercentageRemaining()
{
	return CurrentHealth / MaxHealth; 
}

void UHealthComponent::OnTouchHealthBoost(float HealthAmount)
{
	CurrentHealth += HealthAmount; 
}

void UHealthComponent::UpdateHealthBar()
{
	//If the owner of this health component is an autonomous proxy
	//NOTE: Possible to use function GetOwnerRole() as well!
	//Auto Proxy only works for dedicated servers
	//IsLocallyControlled() allows for listen and dedicated servers to work. 
	APlayerCharacter* OwningCharacter = Cast<APlayerCharacter>(GetOwner()); 
	if (OwningCharacter)
	{
		//Update HealthBar HUD on the controller Player's screen.
		if (OwningCharacter->IsLocallyControlled()) 
		{
			//Find the hud associated to this player
			APlayerHUD* HUD = Cast<APlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
			if (HUD)
			{
				//Update the progress bar widget on the players hud.
				HUD->SetPlayerHealthBarPercent(HealthPercentageRemaining());
			}
		}
		
	}
	
}
