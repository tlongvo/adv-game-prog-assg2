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
	PerceptionComponent = FindComponentByClass<UAIPerceptionComponent>();
	if (!PerceptionComponent) { UE_LOG(LogTemp, Error, TEXT("NO PERCEPTION COMPONENT FOUND")) }
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyCharacter::SensePlayer);

	//Stand-in value to keep Enemy following teammmate
	//As bCanseeTeammate doesn't stay true when seeing teammate 
	bCanFollowTeammate = false; 

	DetectedActor = nullptr; 
	bCanSeeActor = false; 
	bCanSeeTeammate = false; 
	TeammateCharacter = nullptr; 
	HealthComponent = FindComponentByClass<UHealthComponent>();
	MovementComponent = FindComponentByClass<UCharacterMovementComponent>();

	StrafeStartingPoint = GetActorLocation();


	EnemyMesh = GetMesh();
	
	AnimInst = EnemyMesh->GetAnimInstance();
		
	CrouchProp = FindField<UBoolProperty>(AnimInst->GetClass(), "Crouching");


	PathfindingNodeAccuracy = 100.0f;

}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 

	//Only Authority version can hold true behaviour
	if (GetLocalRole() == ROLE_Authority)
	{
		if (CurrentAgentState == AgentState::PATROL)
		{
			AgentPatrol();
			//If enemy can see player and has more than 40% hp, change to ENGAGE 
			if (bCanSeeActor && this->HealthComponent->HealthPercentageRemaining() >= 0.4)
			{
				CurrentAgentState = AgentState::ENGAGE;
				Path.Empty();
			}
			//If enemy senses a hurt teammate, and itself is not below 40% hp, change to FOLLOW
			else if (bCanSeeTeammate && TeammateHealthComponent->HealthPercentageRemaining() < 0.4
				&& HealthComponent->HealthPercentageRemaining() >= 0.4)
			{
				//UE_LOG(LogTemp, Error, TEXT("FOLLOWING FROM PATROL STATE"));
				bCanFollowTeammate = true;
				CurrentAgentState = AgentState::FOLLOW;
				Path.Empty();
			}
			//If can't see enemy but can see Teammate engaging with enemy, go to Teammate if >= 40% hp
			else if (!bCanSeeActor && bCanSeeTeammate && TeammateCharacter->CurrentAgentState == AgentState::ENGAGE
				&& HealthComponent->HealthPercentageRemaining() >= 0.4)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Helping Teammate %s"), *TeammateCharacter->GetName());
				CurrentAgentState = AgentState::FOLLOW;
			}
			else if (bCanSeeActor && this->HealthComponent->HealthPercentageRemaining() < 0.4)
			{
				CurrentAgentState = AgentState::EVADE;
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
			//If enemy sees teammate less than 40% hp, and itself is healthy, follow teammate 
			else if (bCanSeeTeammate && TeammateHealthComponent->HealthPercentageRemaining() < 0.4
				&& HealthComponent->HealthPercentageRemaining() >= 0.4)
			{
				bCanFollowTeammate = true;
				CurrentAgentState = AgentState::FOLLOW;
				Path.Empty();
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
			//ALL: Reset Movement speed back to 600.0f;
			//UE_LOG(LogTemp, Error, TEXT("FOLLOWING STATE"));
			//If healthy & can see the Player and Teammate is recovered, change to ENGAGE
			if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() >= 0.4
				&& TeammateHealthComponent->HealthPercentageRemaining() >= 0.4)
			{
				bCanFollowTeammate = false;
				CurrentAgentState = AgentState::ENGAGE;
				Path.Empty();
				MovementComponent->MaxWalkSpeed = 600.0f;
			}
			//If teammate is recovered, change to PATROL
			//Exclude: if can't see teammate; NOTE: It shouldn't lose sight while following teammate.
			else if ((bCanSeeTeammate && TeammateHealthComponent->HealthPercentageRemaining() >= 0.4))
			{
				bCanFollowTeammate = false;
				CurrentAgentState = AgentState::PATROL;
				MovementComponent->MaxWalkSpeed = 600.0f;
			}
			//If injured and can see the Player, change to EVADE
			else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() < 0.4)
			{
				bCanFollowTeammate = false;
				CurrentAgentState = AgentState::EVADE;
				Path.Empty();
				MovementComponent->MaxWalkSpeed = 600.0f;
			}
		}

		if (CurrentAgentState == AgentState::DODGE)
		{
			Strafe(StrafeStartingPoint);
		}
		else
		{
			MoveAlongPath();
		}
	}
	
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::AgentPatrol() 
{
	if (Path.Num() == 0 && Manager != NULL)
	{
		Path = Manager->GeneratePath(CurrentNode, Manager->AllNodes[FMath::RandRange(0, Manager->AllNodes.Num() - 1)]);
	}
}

void AEnemyCharacter::AgentEngage() 
{
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
	//UE_LOG(LogTemp, Log, TEXT("FOLLOW FUNCTION EXECUTED, %s"), bCanSeeTeammate ? TEXT("true") : TEXT("false"));
	//If permitted to follow teammate
	if (bCanFollowTeammate)
	{	
		//Increase movement speed to keep up with injured teammate.
		MovementComponent->MaxWalkSpeed = 800.0f;

		//Can shoot Player while following teammate
		if (bCanSeeActor)
		{
			FVector DirectionToTarget = DetectedActor->GetActorLocation() - GetActorLocation();
			Fire(DirectionToTarget);
		}

		//If Enemy has no path, Generate a path nearest to teammate.
		if (Path.Num() == 0 && Manager != NULL && TeammateCharacter != NULL)
		{
			ANavigationNode* Nearest = Manager->FindNearestNode(TeammateCharacter->GetActorLocation());
			Path = Manager->GeneratePath(CurrentNode, Nearest);
		} 
	} 
	//Else if not allowed to follow, go to Teammate's location once. 
	else if(!bCanFollowTeammate)
	{
		if (Path.Num() == 0 && Manager != NULL && TeammateCharacter != NULL)
		{
			ANavigationNode* Nearest = Manager->FindNearestNode(TeammateCharacter->GetActorLocation());
			Path = Manager->GeneratePath(CurrentNode, Nearest);
		}
	}
}

void AEnemyCharacter::SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus)
{
	//Ensure Authority version can only sense, then replicated this down
	if (GetLocalRole() == ROLE_Authority)
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			//Check if Actor sensed is another EnemyCharacter
			if (Cast<AEnemyCharacter>(ActorSensed))
			{
				bCanSeeTeammate = true;
				TeammateCharacter = Cast<AEnemyCharacter>(ActorSensed); //Cast the Actor class to the child EnemyCharacter Class 

				//Get Teammate's Health Information
				TeammateHealthComponent = TeammateCharacter->HealthComponent;
				UE_LOG(LogTemp, Warning, TEXT("ENEMY Detected: %s"), *TeammateCharacter->GetName());
			}
			//Check if Actor sensed is the PlayerCharacter
			else if (Cast<APlayerCharacter>(ActorSensed))
			{
				DetectedActor = ActorSensed;
				bCanSeeActor = true;
				UE_LOG(LogTemp, Warning, TEXT("PLAYER Detected"));
			}
		}
		else
		{
			bCanSeeActor = false;
			bCanSeeTeammate = false;
			UE_LOG(LogTemp, Warning, TEXT("Player Lost"))
		}
	}
	
}

void AEnemyCharacter::OnHit()
{
	UE_LOG(LogTemp, Warning, TEXT("ENEMY IS DODGE MODE"));
	StrafeStartingPoint = GetActorLocation();
	CurrentAgentState = AgentState::DODGE;
}

void AEnemyCharacter::Strafe(FVector StartingPoint)
{
	FVector Distance = (GetActorLocation() - StartingPoint);

	if (Distance.Size() > 50.0f) // If out of bounds, return to start
	{
		UE_LOG(LogTemp, Warning, TEXT("ENEMY IS RETURNING TO START"));
		goingToStart = true;
		MovementComponent->UnCrouch();

		if (CrouchProp != NULL)
		{
			CrouchProp->SetPropertyValue_InContainer(AnimInst, false);
		}
	}
	else if (Distance.IsNearlyZero(10.0f)) // If at start, get new random location to strafe to 
	{
		UE_LOG(LogTemp, Warning, TEXT("ENEMY IS DODGING, %f away from starting point"), Distance.Size());
		goingToStart = false;
		RandomLocation = FVector::FVector(GetActorLocation().X + FMath::RandRange(-500.0f, 500.0f), GetActorLocation().Y + FMath::RandRange(-500.0f, 500.0f), GetActorLocation().Z);
		MovementComponent->Crouch();

		if (CrouchProp != NULL)
		{
			CrouchProp->SetPropertyValue_InContainer(AnimInst, true);
		}
	}

	if (goingToStart == true) // Return to the start
	{
		FVector Direction = StartingPoint - GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("ENEMY IS GOING TO START"));

		Direction.Normalize();
		AddMovementInput(Direction, 1.0f);
	}
	else if (goingToStart == false) // Strafe to direction
	{
		FVector Direction = RandomLocation - GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("ENEMY IS GOING TO %s"), *RandomLocation.ToString());

		Direction.Normalize();
		AddMovementInput(Direction, 1.0f);
	}

	if (bCanSeeActor && this->HealthComponent->HealthPercentageRemaining() >= 0.4) // Turn to the player while strafing if visible and health is over 40%
	{
		FVector DirectionToTarget = DetectedActor->GetActorLocation() - GetActorLocation();
		Fire(DirectionToTarget);

		//Get the AI to face in the direction of travel.
		FRotator FaceDirection = DirectionToTarget.ToOrientationRotator();
		FaceDirection.Roll = 0.f;
		FaceDirection.Pitch = 0.f;
		//FaceDirection.Yaw -= 90.0f;
		SetActorRotation(FaceDirection);
	}
	else //Stop dodging
	{
		if (CrouchProp != NULL)
		{
			CrouchProp->SetPropertyValue_InContainer(AnimInst, false); // Stop crouching
		}
		//CurrentAgentState = AgentState::ENGAGE;
		//If stop dodging, it should be somewhat clear of danger, thus back to patrol state
		CurrentAgentState = AgentState::PATROL;
	}
}

void AEnemyCharacter::MoveAlongPath()
{
	if (Path.Num() > 0 && Manager != NULL)
	{
		//UE_LOG(LogTemp, Display, TEXT("Current Node: %s"), *CurrentNode->GetName())
		//If Character has reached one of the nodes along the path.
		if ((GetActorLocation() - CurrentNode->GetActorLocation()).IsNearlyZero(PathfindingNodeAccuracy))
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


