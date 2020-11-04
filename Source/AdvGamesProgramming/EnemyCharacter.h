// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationNode.h"
#include "GameFramework/Character.h"
#include "AIManager.h"
#include "HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "PlayerCharacter.h"
#include "TimerManager.h" 
#include "EnemyCharacter.generated.h"

UENUM()
enum class AgentState : uint8
{
	PATROL,
	ENGAGE,
	EVADE,
	FOLLOW,
	DODGE
};

UCLASS()
class ADVGAMESPROGRAMMING_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<ANavigationNode*> Path;
	ANavigationNode* CurrentNode;
	AAIManager* Manager;
	UAIPerceptionComponent* PerceptionComponent; 
	AActor* DetectedActor;
	bool bCanSeeActor; 
	bool bCanSeeTeammate;
	bool bCanFollowTeammate; 
	UHealthComponent* HealthComponent;
	UHealthComponent* TeammateHealthComponent; 
	UCharacterMovementComponent* MovementComponent; 
	AEnemyCharacter* TeammateCharacter; 

	UPROPERTY(VisibleAnywhere)
		AgentState CurrentAgentState;

	FVector StrafeStartingPoint;
	bool goingToStart;
	FVector RandomLocation;
	USkeletalMeshComponent* EnemyMesh;
	UAnimInstance* AnimInst;
	UBoolProperty* CrouchProp;

	//Sense new Player Delay
	FTimerHandle SenseDelayHandle; 
	bool bCanSense; 
	void AllowSensing(); 

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void AgentPatrol();
	void AgentEngage();
	void AgentEvade(); 
	void AgentFollow();

	UFUNCTION()
	void SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus);

	UFUNCTION(BlueprintImplementableEvent)
	void Fire(FVector FireDirection);

	UFUNCTION(BlueprintCallable)
	void OnHit();

	void Strafe(FVector StartingPoint);

	UPROPERTY(EditAnywhere, meta = (UIMin = "10.0", UIMax = "1000.0", ClampMin = "10.0", ClampMax = "1000.0"))
	float PathfindingNodeAccuracy;

private:
	void MoveAlongPath();
	
};
