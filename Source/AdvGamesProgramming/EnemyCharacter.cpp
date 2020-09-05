// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EngineUtils.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentAgentState = AgentState::PATROL;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	//attach AI Perception Components to variable 
	PerceptionComponent = FindComponentByClass<UAIPerceptionComponent>();
	if (!PerceptionComponent) { UE_LOG(LogTemp, Error, TEXT("NO PERCEPTION COMPONENT FOUND")) }
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyCharacter::SensePlayer);
	
	DetectedActor = nullptr; 
	bCanSeeActor = false; 
	bCanSeeTeammate = false; 
	HealthComponent = FindComponentByClass<UHealthComponent>();
	MovementComponent = FindComponentByClass<UCharacterMovementComponent>(); 
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Fire(FVector::ZeroVector);
	if (CurrentAgentState == AgentState::PATROL)
	{
		AgentPatrol();
		if (bCanSeeActor && this->HealthComponent->HealthPercentageRemaining() >= 0.4)
		{
			CurrentAgentState = AgentState::ENGAGE;
			Path.Empty();
		}
		else if (bCanSeeActor && this->HealthComponent->HealthPercentageRemaining() < 0.4)
		{
			CurrentAgentState = AgentState::EVADE;
			Path.Empty();
		}
		else if (bCanSeeTeammate && TeammateHealthComponent->HealthPercentageRemaining() <= 0.4)
		{

			//if enemy sees teammate and teammate is hurt, go to it and follow it.
			//engage state placeholder, its basically a follow function
			UE_LOG(LogTemp, Error, TEXT("Helping Teammate from Patrol"));
			CurrentAgentState = AgentState::FOLLOW; 
			
		}
		MoveAlongPath();
	}
	else if (CurrentAgentState == AgentState::ENGAGE)
	{
		AgentEngage();
		if (!bCanSeeActor)
		{
			CurrentAgentState = AgentState::PATROL;
		}
		else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() < 0.4)
		{
			CurrentAgentState = AgentState::EVADE;
			Path.Empty();
		}
		MoveAlongPath();
	}
	else if (CurrentAgentState == AgentState::EVADE)
	{
		AgentEvade();
		if (!bCanSeeActor)
		{
			CurrentAgentState = AgentState::PATROL;
		}
		else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() >= 0.4)
		{
			CurrentAgentState = AgentState::ENGAGE;
			Path.Empty();
		}
		MoveAlongPath();
	}
	else if (CurrentAgentState == AgentState::FOLLOW)
	{
		AgentFollow();
		//If can see actor, and teammate has recovered, engage actor
		//if can't see actor, and teamamate has recovered, go patrol,
		//if can see actor, and teammate hasnt recovered, and low hp, evade (but teammate also is evading, so it can still follow?)
		//maybe copy teammates path 

		MoveAlongPath();
	}

	
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::AgentPatrol() {
	if (Path.Num() == 0 && Manager != NULL)
	{
		Path = Manager->GeneratePath(CurrentNode, Manager->AllNodes[FMath::RandRange(0, Manager->AllNodes.Num() - 1)]);
	}
}

void AEnemyCharacter::AgentEngage() {

	if (bCanSeeActor)
	{
		if (Path.Num() == 0 && Manager != NULL && DetectedActor != NULL)
		{
			ANavigationNode* Nearest = Manager->FindNearestNode(DetectedActor->GetActorLocation());
			Path = Manager->GeneratePath(CurrentNode, Nearest);
		}
			FVector DirectionToTarget = DetectedActor->GetActorLocation() - GetActorLocation();
			Fire(DirectionToTarget);
	}
}

void AEnemyCharacter::AgentEvade()
{
	if (Path.Num() == 0 && Manager != NULL && DetectedActor != NULL)
	{
		Path = Manager->GeneratePath(CurrentNode, Manager->FindFurthestNode(DetectedActor->GetActorLocation()));
		
	}

	if (bCanSeeActor)
	{
		FVector DirectionToTarget = DetectedActor->GetActorLocation() - GetActorLocation();
		Fire(DirectionToTarget);
	}
}

void AEnemyCharacter::AgentFollow() 
{
	//check if Enemy has no set path
	//Assign new path to Teammate
	if (bCanSeeTeammate)
	{	
		//increase movement speed, to keep up with teammate, 
		MovementComponent->MaxWalkSpeed = 1000.0f; 
		if (Path.Num() == 0 && Manager != NULL && DetectedActor != NULL)
		{
			ANavigationNode* Nearest = Manager->FindNearestNode(DetectedActor->GetActorLocation());
			Path = Manager->GeneratePath(CurrentNode, Nearest);
		} 
		
	}
	
}

void AEnemyCharacter::SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus) 
{
	if (Stimulus.WasSuccessfullySensed())
	{
		DetectedActor = ActorSensed;
		//Check if Actor is the Player or Enemy
		
		if (DetectedActor->GetName() == FString("PlayerCharacterBlueprint_2")) 
		{
			bCanSeeActor = true;
		}
		//If Enemy sees another EnemyCharacter
		//Gather their Health Information
		else if (DetectedActor->GetClass()->GetName() == FString("EnemyCharacterBlueprint_C"))
		{
			bCanSeeTeammate = true; 
			//Copy Teammate's HealthComponent
			TeammateHealthComponent = DetectedActor->FindComponentByClass<UHealthComponent>();
			//UE_LOG(LogTemp, Warning, TEXT("Enemy Detected: %s"), *DetectedActor->GetName());
		}
		
		//Check if Health Component exists on Detected Actor
		//Note: Player doesn't have a health component
		//if(DetectedActor->FindComponentByClass<UHealthComponent>() != nullptr)
		//{
		//.
		//UE_LOG(LogTemp, Warning, TEXT("Component Detected: %s"), *DetectedActor->GetClass()->GetName());
		//}
		
	}
	else
	{
		bCanSeeActor = false; 
		bCanSeeTeammate = false; 
		UE_LOG(LogTemp, Warning, TEXT("Actor Lost"));
	}
}

void AEnemyCharacter::MoveAlongPath()
{
	if (Path.Num() > 0 && Manager != NULL)
	{
		//UE_LOG(LogTemp, Display, TEXT("Current Node: %s"), *CurrentNode->GetName())
		if ((GetActorLocation() - CurrentNode->GetActorLocation()).IsNearlyZero(100.0f))
		{
			UE_LOG(LogTemp, Display, TEXT("At Node %s"), *CurrentNode->GetName())
				CurrentNode = Path.Pop();
		}
		else
		{
			FVector WorldDirection = CurrentNode->GetActorLocation() - GetActorLocation();
			WorldDirection.Normalize();
			//UE_LOG(LogTemp, Display, TEXT("The World Direction(X:%f,Y:%f,Z:%f)"), WorldDirection.X, WorldDirection.Y, WorldDirection.Z)
			AddMovementInput(WorldDirection, 1.0f);

			//Get the AI to face in the direction of travel.
			FRotator FaceDirection = WorldDirection.ToOrientationRotator();
			FaceDirection.Roll = 0.f;
			FaceDirection.Pitch = 0.f;
			//FaceDirection.Yaw -= 90.0f;
			SetActorRotation(FaceDirection);
		}
	}
}


