// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"
#include "EngineUtils.h"
#include "EnemyCharacter.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AAIManager::AAIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AllowedAngle = 0.4f; 
}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();
	
	PopulateNodes();
	//CreateAgents(); //Multiplayer - Called in the gamemode 
}

// Called every frame
void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<ANavigationNode*> AAIManager::GeneratePath(ANavigationNode* StartNode, ANavigationNode* EndNode)
{

	TArray<ANavigationNode*> OpenSet;
	for (ANavigationNode* Node : AllNodes)
	{
		Node->GScore = TNumericLimits<float>::Max();
	}

	StartNode->GScore = 0;
	StartNode->HScore = FVector::Distance(StartNode->GetActorLocation(), EndNode->GetActorLocation());

	OpenSet.Add(StartNode);

	while (OpenSet.Num() > 0)
	{
		int32 IndexLowestFScore = 0;
		for (int32 i = 1; i < OpenSet.Num(); i++)
		{
			if (OpenSet[i]->FScore() < OpenSet[IndexLowestFScore]->FScore())
			{
				IndexLowestFScore = i;
			}
		}
		ANavigationNode* CurrentNode = OpenSet[IndexLowestFScore];

		OpenSet.Remove(CurrentNode);

		if (CurrentNode == EndNode) {
			TArray<ANavigationNode*> Path;
			Path.Push(EndNode);
			CurrentNode = EndNode;
			while (CurrentNode != StartNode)
			{
				CurrentNode = CurrentNode->CameFrom;
				Path.Add(CurrentNode);
			}
			return Path;
		}

		for (ANavigationNode* ConnectedNode : CurrentNode->ConnectedNodes)
		{
			float TentativeGScore = CurrentNode->GScore + FVector::Distance(CurrentNode->GetActorLocation(), ConnectedNode->GetActorLocation());
			if (TentativeGScore < ConnectedNode->GScore)
			{
				ConnectedNode->CameFrom = CurrentNode;
				ConnectedNode->GScore = TentativeGScore;
				ConnectedNode->HScore = FVector::Distance(ConnectedNode->GetActorLocation(), EndNode->GetActorLocation());
				if (!OpenSet.Contains(ConnectedNode))
				{
					OpenSet.Add(ConnectedNode);
				}
			}
		}
	}

	//If it leaves this loop without finding the end node then return an empty path.
	UE_LOG(LogTemp, Error, TEXT("NO PATH FOUND"));
	return TArray<ANavigationNode*>();
}

void AAIManager::PopulateNodes()
{
	for (TActorIterator<ANavigationNode> It(GetWorld()); It; ++It)
	{
		AllNodes.Add(*It);
	}
}

void AAIManager::CreateAgents()
{
	for (int32 i = 0; i < NumAI; i++)
	{
		int32 RandIndex = FMath::RandRange(0, AllNodes.Num()-1);
		//Height spawn offset to prevent spawn inside ground.
		AEnemyCharacter* Agent = GetWorld()->SpawnActor<AEnemyCharacter>(AgentToSpawn, 
			AllNodes[RandIndex]->GetActorLocation() + FVector(0.0f,0.0f, 500.0f), FRotator(0.f, 0.f, 0.f));
		Agent->Manager = this;
		Agent->CurrentNode = AllNodes[RandIndex];
		AllAgents.Add(Agent);
	}
}

void AAIManager::GenerateNodes(const TArray<FVector>& Vertices, int32 Width, int32 Height)
{
	AllNodes.Empty();

	for (TActorIterator<ANavigationNode> It(GetWorld()); It; ++It)
	{
		It->Destroy();
	}

	for (int32 Row = 0; Row < Height; Row++)
	{
		for (int32 Col = 0; Col < Width; Col++)
		{
			//Create and add the nodes to the AllNodes array.
			AllNodes.Add(GetWorld()->SpawnActor<ANavigationNode>(Vertices[Row * Width + Col], FRotator::ZeroRotator, FActorSpawnParameters()));
		}
	}

	for (int32 Row = 0; Row < Height; Row++)
	{
		for (int32 Col = 0; Col < Width; Col++)
		{
			//Add the connections.

			// CORNER CASES:
			if (Row == 0 && Col == 0)
			{
				//   - Bottom Corner where Row = 0 and Col = 0
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
			}
			else if (Row == 0 && Col == Width - 1)
			{
				//   - Bottom Corner where Row = 0 and Col = Width - 1
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
			}
			else if (Row == Height - 1 && Col == 0)
			{
				//   - Top Corner where Row = Height - 1 and Col = 0
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
			}
			else if (Row == Height - 1 && Col == Width - 1)
			{
				//   - Top Corner where Row = Height - 1 and Col = Width - 1
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
			}
			// EDGE CASES:
			else if (Col == 0)
			{
				//   - Left Edge where Col = 0
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col + 1)]);
			}
			else if (Row == Height - 1)
			{
				//   - Top Edge where Row = Height - 1
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col + 1)]);
			}
			else if (Col == Width - 1)
			{
				//   - Right Edge where Col = Width - 1
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col - 1)]);
			}
			else if (Row == 0)
			{
				//   - Bottom Edge where Row = 0
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col - 1)]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col + 1)]);
			}
			// NORMAL CASES
			else
			{
				//Connect Top Left
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col - 1)]);
				//Connect Top
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + Col]);
				//Connect Top Right
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row + 1) * Width + (Col + 1)]);
				//Connect Middle Left
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col - 1)]);
				//Connect Middle Right
				AddConnection(AllNodes[Row * Width + Col], AllNodes[Row * Width + (Col + 1)]);
				//Connect Bottom Left
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col - 1)]);
				//Connect Bottom Middle
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + Col]);
				//Connect Bottom Right
				AddConnection(AllNodes[Row * Width + Col], AllNodes[(Row - 1) * Width + (Col + 1)]);
			}
		}
	}
}

void AAIManager::AddConnection(ANavigationNode* FromNode, ANavigationNode* ToNode)
{
	FVector Direction = FromNode->GetActorLocation() - ToNode->GetActorLocation();
	Direction.Normalize();
	if (Direction.Z < AllowedAngle && Direction.Z > AllowedAngle * -1.0f)
	{
		FromNode->ConnectedNodes.Add(ToNode);
	}

}

//Finds nearest nodes by checking the distance between all nodes
//And choosing the one that is the closest
ANavigationNode* AAIManager::FindNearestNode(const FVector& Location)
{
	ANavigationNode* NearestNode = nullptr;
	float NearestDistance = TNumericLimits<float>::Max();
	//Loop through the nodes and find the nearest one in distance
	for (ANavigationNode* CurrentNode : AllNodes)
	{
		float CurrentNodeDistance = FVector::Distance(Location, CurrentNode->GetActorLocation());
		if (CurrentNodeDistance < NearestDistance)
		{
			NearestDistance = CurrentNodeDistance;
			NearestNode = CurrentNode;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Nearest Node: %s"), *NearestNode->GetName())
	return NearestNode;
}

ANavigationNode* AAIManager::FindFurthestNode(const FVector& Location)
{
	ANavigationNode* FurthestNode = nullptr;
	float FurthestDistance = 0.0f;
	//Loop through the nodes and find the nearest one in distance
	for (ANavigationNode* CurrentNode : AllNodes)
	{
		float CurrentNodeDistance = FVector::Distance(Location, CurrentNode->GetActorLocation());
		if (CurrentNodeDistance > FurthestDistance)
		{
			FurthestDistance = CurrentNodeDistance;
			FurthestNode = CurrentNode;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Furthest Node: %s"), *FurthestNode->GetName())
	return FurthestNode;
}

