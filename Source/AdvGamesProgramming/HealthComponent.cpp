// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "EnemyCharacter.h"
#include "Engine/GameEngine.h"
#include "PlayerCharacter.h"
#include "PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "ClientPlayerState.h"
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
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//Health Regen
	CurrentHealth += 0.0225;
	UpdateHealthBar();
	
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

void UHealthComponent::OnTakeDamage(float Damage, AActor* Attacker) 
{
	//Current function is called if its owner is from the server 

	CurrentHealth -= Damage;
	if (CurrentHealth <= 0)
	{
		CurrentHealth = 0;
		OnDeath();
		UpdateAttackerKillCount(Attacker);
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		UpdateHealthBar();
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
	if (GetOwnerRole() == ROLE_Authority)
	{
		UpdateHealthBar();
	}
}

void UHealthComponent::UpdateHealthBar()
{
	//If the owner of this health component is an autonomous proxy
	//NOTE: Possible to use function GetOwnerRole() as well!
	//Autonomous Proxy only works for dedicated servers
	//IsLocallyControlled() allows for listen and dedicated servers to work. 
	APlayerCharacter* OwningCharacter = Cast<APlayerCharacter>(GetOwner()); 
	if (OwningCharacter)
	{
		//Update HealthBar HUD on the controller Player's screen.
		if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy || (GetOwnerRole() == ROLE_Authority && Cast<APawn>(GetOwner())->IsLocallyControlled()))
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

void UHealthComponent::UpdateAttackerKillCount(AActor* Attacker)
{
	//Attacker is always an authority when called from fire function in gun blueprint
	if (APlayerCharacter* AttackerCharacter = Cast<APlayerCharacter>(Attacker))
	{
		if (AttackerCharacter->GetLocalRole() == ROLE_Authority)
		{
			UE_LOG(LogTemp, Display, TEXT("Attacker is authority"));
			APlayerController* AttackerController = Cast<APlayerController>(AttackerCharacter->GetController());
			if (AttackerController)
			{
				UE_LOG(LogTemp, Display, TEXT("Attacker has valid controller"));
				AClientPlayerState* ClientState = Cast<AClientPlayerState>(AttackerController->PlayerState);
				if (ClientState)
				{
					UE_LOG(LogTemp, Display, TEXT("Attacker ClientPlayerState is valid"));
					//Update KillCount on Authority Version of the Attacking Player
					ClientState->KillCount++;
					//Update the HUD of Attacking Player
					//RPC call to Attacker Client (Non-Authority version) to Update the HUD
					AttackerCharacter->UpdateKillsHUD(ClientState->KillCount);
				}
			}
		}
	}
	
}