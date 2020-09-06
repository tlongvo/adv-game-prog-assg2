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
		//If enemy can see player and has < 40% HP
		if (bCanSeeActor && this->HealthComponent->HealthPercentageRemaining() >= 0.4 )
		{
			CurrentAgentState = AgentState::ENGAGE;
			Path.Empty();
		}
		else if (bCanSeeActor && this->HealthComponent->HealthPercentageRemaining() < 0.4)
		{
			CurrentAgentState = AgentState::EVADE;
			Path.Empty();
		}
		else if (bCanSeeTeammate && TeammateHealthComponent->HealthPercentageRemaining() < 0.4 
			&& this->HealthComponent->HealthPercentageRemaining() >= 0.4)
		{
			//IF enemy senses a hurt teammate, and itself is not below 40% hp, then follow hurt teammate.
			CurrentAgentState = AgentState::FOLLOW; 
			Path.Empty();
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
		//If teammate is recovered, change to PATROL
		//OR if can't see teammate, change to PATROL
		if((bCanSeeTeammate && TeammateHealthComponent->HealthPercentageRemaining() >= 0.4) || !bCanSeeTeammate)
		{
			CurrentAgentState = AgentState::PATROL;
			MovementComponent->MaxWalkSpeed = 600.0f;
		}
		//If < 40% hp and can see the Player, change to EVADE
		else if (bCanSeeActor && this->HealthComponent->HealthPercentageRemaining() < 0.4)
		{
			CurrentAgentState = AgentState::EVADE; 
			MovementComponent->MaxWalkSpeed = 600.0f;
		}
		//If > 40% hp and can see the Player, change to ENGAGE
		else if (bCanSeeActor && this->HealthComponent->HealthPercentageRemaining() >= 0.4)
		{
			CurrentAgentState = AgentState::ENGAGE; 
			MovementComponent->MaxWalkSpeed = 600.0f;
		}
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
	//UE_LOG(LogTemp, Log, TEXT("ENGAGE FUNCTION EXECUTED, %s"), bCanSeeActor ? TEXT("true") : TEXT("false"));
	if (bCanSeeActor)
	{
		FVector DirectionToTarget = DetectedActor->GetActorLocation() - GetActorLocation();
		Fire(DirectionToTarget);

		if (Path.Num() == 0 && Manager != NULL && DetectedActor != NULL)
		{
			ANavigationNode* Nearest = Manager->FindNearestNode(DetectedActor->GetActorLocation());
			Path = Manager->GeneratePath(CurrentNode, Nearest);
		}
			
	}
}

void AEnemyCharacter::AgentEvade()
{
	if (bCanSeeActor)
	{
		FVector DirectionToTarget = DetectedActor->GetActorLocation() - GetActorLocation();
		Fire(DirectionToTarget);

		if (Path.Num() == 0 && Manager != NULL && DetectedActor != NULL)
		{
			Path = Manager->GeneratePath(CurrentNode, Manager->FindFurthestNode(DetectedActor->GetActorLocation()));
		}
	}
}

void AEnemyCharacter::AgentFollow() 
{
	//check if Enemy has no set path
	//Assign new path to Teammate
	UE_LOG(LogTemp, Log, TEXT("FOLLOW FUNCTION EXECUTED, %s"), bCanSeeTeammate ? TEXT("true") : TEXT("false"));
	if (bCanSeeTeammate)
	{	
		//UE_LOG(LogTemp, Error, TEXT("Can See Teammate"));
		MovementComponent->MaxWalkSpeed = 800.0f;
		//increase movement speed, to keep up with teammate, 
		if (Path.Num() == 0 && Manager != NULL && DetectedActor != NULL)
		{
			//find node nearest to teammate 
			UE_LOG(LogTemp, Error, TEXT("Path EMPTY -> Generating Path"));
			ANavigationNode* Nearest = Manager->FindNearestNode(DetectedActor->GetActorLocation());
			Path = Manager->GeneratePath(CurrentNode, Nearest);
		} 
	} 

	/*
	 Shoot Player while following teammate 
	if (bCanSeeActor)
	{
		FVector DirectionToTarget = DetectedActor->GetActorLocation() - GetActorLocation();
		Fire(DirectionToTarget);
	}
	*/
	
}

void AEnemyCharacter::SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		
		//Check if Actor is the Player or Enemy
		if (ActorSensed->GetName() == FString("PlayerCharacterBlueprint_2")) 
		{
			DetectedActor = ActorSensed;
			bCanSeeActor = true;
			UE_LOG(LogTemp, Warning, TEXT("SEE PLAYER"));
		} 
		//If Enemy sees another EnemyCharacter
		//Gather their Health Information
		else if (ActorSensed->GetClass()->GetName() == FString("EnemyCharacterBlueprint_C"))
			//else if (DetectedActor->GetName() == FString("EnemyCharacterBlueprint_C_0") || DetectedActor->GetName() == FString("EnemyCharacterBlueprint_C_1"))
		{
			DetectedActor = ActorSensed;
			bCanSeeTeammate = true;
			//Copy Teammate's HealthComponent
			TeammateHealthComponent = DetectedActor->FindComponentByClass<UHealthComponent>();
			UE_LOG(LogTemp, Warning, TEXT("Enemy Detected: %s"), *DetectedActor->GetName());
		}

		//Check if Health Component exists on Detected Actor
		//Note: Player doesn't have a health component
		//if(DetectedActor->FindComponentByClass<UHealthComponent>() != nullptr)
		//{
		//.
		//UE_LOG(LogTemp, Warning, TEXT("Component Detected: %s"), *DetectedActor->GetClass()->GetName());
		//}

	}
	else //if (!Stimulus.WasSuccessfullySensed())
	{
		bCanSeeActor = false;
		bCanSeeTeammate = false; 
		UE_LOG(LogTemp, Warning, TEXT("Player Lost"))
	}
}

void AEnemyCharacter::MoveAlongPath()
{
	if (Path.Num() > 0 && Manager != NULL)
	{
		//UE_LOG(LogTemp, Display, TEXT("Current Node: %s"), *CurrentNode->GetName())
		//If Character has reached one of the nodes along the path.
		if ((GetActorLocation() - CurrentNode->GetActorLocation()).IsNearlyZero(100.0f))
		{
			UE_LOG(LogTemp, Display, TEXT("Currently At Node %s"), *CurrentNode->GetName())
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


