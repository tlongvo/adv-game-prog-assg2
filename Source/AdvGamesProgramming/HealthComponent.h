// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVGAMESPROGRAMMING_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(ReplicatedUsing = UpdateHealthBar, VisibleAnywhere, BlueprintReadOnly)
	float CurrentHealth;

	UPROPERTY(Replicated)
	bool bIsDead;

	UFUNCTION(BlueprintCallable)
	void OnTakeDamage(float Damage, AActor* Attacker);

	UFUNCTION(BlueprintCallable)
	void OnDeath(AActor* Attacker);
	
	UFUNCTION(BlueprintCallable)
	void CheckHealth(AActor* Attacker);

	//UPROPERTY(Replicated)
	AActor* AttackingPlayer; 

	UFUNCTION(Server, Reliable)
	void ServerSetAttacker(AActor* Attacker);

	float HealthPercentageRemaining();

	void OnTouchHealthBoost(float HealthAmount);

private:
	UFUNCTION()
		void UpdateHealthBar();

	void UpdateAttackerKillCount(AActor* Attacker);

	

};
