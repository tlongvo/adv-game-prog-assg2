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
		else if (bCanSeeTeammate)
		{

		}
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
	}

	MoveAlongPath();
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

void AEnemyCharacter::SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus) 
{
	if (Stimulus.WasSuccessfullySensed())
	{
		DetectedActor = ActorSensed;
		//bCanSeeActor = true; 
		//Check if Actor is the Player or Enemy
		
		if (DetectedActor->GetName() == FString("PlayerCharacterBlueprint_2")) 
		{
			bCanSeeActor = true;
		}
		//If Enemy detects another enemy type or teammate
		else if (DetectedActor->GetName() == FString("EnemyCharacterBlueprint_C_0")
			|| DetectedActor->GetName() == FString("EnemyCharacterBlueprint_C_1"))
		{
			bCanSeeTeammate = true; 
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Player Detected %s"), *DetectedActor->GetName());
	}
	else
	{
		bCanSeeActor = false; 
		bCanSeeTeammate = false; 
		UE_LOG(LogTemp, Warning, TEXT("Player Lost"));
	}
}

void AEnemyCharacter::OnHit()
{

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


