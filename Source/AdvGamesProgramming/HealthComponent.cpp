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
	bIsDead = false; 
	
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
	//DOREPLIFETIME(UHealthComponent, AttackingPlayer);
	//DOREPLIFETIME(UHealthComponent, bIsDead);
}

void UHealthComponent::OnTakeDamage(float Damage, AActor* Attacker) 
{
	//Current function is called if its owner is from the server 

	CurrentHealth -= Damage;
	if (CurrentHealth <= 0)
	{
		CurrentHealth = 0;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		UpdateHealthBar();
	}
	//If the Attack occured on Server
	//if attacker is authority then attack occurred on server
	//so health stuff would be replicated down to the clients here. 
	/*
	if (Attacker->GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Display, TEXT("attacker is AUTHORITY"));
		//Calls in listen servers where Player is also a host (server)
		if (GetOwnerRole() == ROLE_Authority)
		{
			UpdateHealthBar();
		}
		CurrentHealth -= Damage;
		//UE_LOG(LogTemp, Display, TEXT("SERVER VERION  Health %f"), HealthPercentageRemaining());
		AttackingPlayer = Attacker;

		if (CurrentHealth <= 0)
		{
			CurrentHealth = 0;
			UpdateAttackerKillCount(Attacker);
			OnDeath(Attacker);
			
		}
	}
	else if (Attacker->GetLocalRole() == ROLE_AutonomousProxy)
	{
		//If victim is dead 
		UE_LOG(LogTemp, Display, TEXT("attacker IS autonomous proxy Health %f"), HealthPercentageRemaining());
		CurrentHealth -= Damage; 
		if (HealthPercentageRemaining() <= 0.0f)
		{
			UpdateAttackerKillCount(Attacker);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("PLAYER IS NOT DEAD"));
		}
	}
	*/

}

void UHealthComponent::OnDeath(AActor* Attacker)
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
	//Auto Proxy only works for dedicated servers
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

void UHealthComponent::CheckHealth(AActor* Attacker)
{
	if (Attacker->GetLocalRole() == ROLE_Authority)
	{
		if (CurrentHealth <= 0)
		{
			UE_LOG(LogTemp, Display, TEXT("attacker IS AUTHORITY Health %f"), HealthPercentageRemaining());
			UpdateAttackerKillCount(Attacker);
			OnDeath(Attacker);
		}
		
	}
	else if (Attacker->GetLocalRole() == ROLE_AutonomousProxy) //If Attack is occur locally, get client (not server)
	{
		if (Cast<APlayerCharacter>(Attacker)->IsLocallyControlled())
		{
			if (CurrentHealth <= 1)
			{
				UE_LOG(LogTemp, Display, TEXT("attacker IS autonomous proxy Health %f"), HealthPercentageRemaining());
				//UpdateAttackerKillCount(Attacker);
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
			UE_LOG(LogTemp, Display, TEXT("Attacker is valid authority"));
			APlayerController* AttackerController = Cast<APlayerController>(AttackerCharacter->GetController());
			if (AttackerController)
			{
				UE_LOG(LogTemp, Display, TEXT("Attacker is valid authority"));
				AClientPlayerState* ClientState = Cast<AClientPlayerState>(AttackerController->PlayerState);
				if (ClientState)
				{
					UE_LOG(LogTemp, Display, TEXT("Client State is Valid"));
					++ClientState->KillCount;
					AttackerCharacter->UpdateKillsHUD(ClientState->KillCount);
					//Currently Attacker is an authority
					//so when we call UpdatekillsHUD, it is an rpc to the client to update their HUD
					//the function should grab the clientstate->killcount 
					
					//HUD->SetKillsText(ClientState->KillCount);
				}

				/*
				UE_LOG(LogTemp, Display, TEXT("Attacker controller is valid %s"), *AttackerController->GetName());
				APlayerHUD* HUD = Cast<APlayerHUD>(AttackerController->GetHUD());
				if (HUD)
				{
					UE_LOG(LogTemp, Display, TEXT("GOT HUD"));
					
				}
				*/
			}
			
		}
		
	}
}

void UHealthComponent::ServerSetAttacker_Implementation(AActor* Attacker)
{
	AttackingPlayer = Attacker; 
}
