// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedurallyGeneratedMap.h"

// Sets default values
AProcedurallyGeneratedMap::AProcedurallyGeneratedMap()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh Component"));
	PerlinScale = 1000.0f;
	PerlinRoughness = 0.1f;
	bRegenerateMap = false;
}

// Called when the game starts or when spawned
void AProcedurallyGeneratedMap::BeginPlay()
{
	Super::BeginPlay();
	/*
	TArray<FVector> Vertices;
	Vertices.Add(FVector(0.0f, 0.0f, 0.0f));
	Vertices.Add(FVector(0.0f, 100.0f, 0.0f));
	Vertices.Add(FVector(100.0f, 100.0f, 0.0f));
	Vertices.Add(FVector(100.0f, 0.0f, 0.0f));

	TArray<int32> Triangles;
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(3);
	Triangles.Add(3);
	Triangles.Add(1);
	Triangles.Add(2);

	TArray<FVector2D> UVCoords;
	UVCoords.Add(FVector2D(0.0f, 0.0f));
	UVCoords.Add(FVector2D(0.0f, 1.0f));
	UVCoords.Add(FVector2D(1.0f, 1.0f));
	UVCoords.Add(FVector2D(1.0f, 0.0f));

	MeshComponent->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UVCoords, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	*/

}

// Called every frame
void AProcedurallyGeneratedMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRegenerateMap)
	{
		ClearMap();
		GenerateMap();
		bRegenerateMap = false;
	}
}

bool AProcedurallyGeneratedMap::ShouldTickIfViewportsOnly() const
{
	return true;
}

void AProcedurallyGeneratedMap::GenerateMap()
{
	float PerlinOffset = FMath::RandRange(-10000.0f, 10000.0f);

	for (int Row = 0; Row < Height; Row++)
	{
		for (int Col = 0; Col < Width; Col++)
		{
			float X = Col * GridSize;
			float Y = Row * GridSize;
			float Z = FMath::PerlinNoise2D(FVector2D(float(Col) * PerlinRoughness + PerlinOffset, float(Row) * PerlinRoughness + PerlinOffset)) * PerlinScale;
			Vertices.Add(FVector(X, Y, Z));
			UVCoords.Add(FVector2D(Col, Row));

		}
	}

	for (int Row = 0; Row < Height - 1; Row++)
	{
		for (int Col = 0; Col < Width - 1; Col++)
		{
			Triangles.Add(Row * Width + Col);
			Triangles.Add((Row + 1) * Width + Col);
			Triangles.Add((Row * Width) + Col + 1);
			Triangles.Add((Row * Width) + Col + 1);
			Triangles.Add((Row + 1) * Width + Col);
			Triangles.Add((Row + 1) * Width + Col + 1);
		}
	}

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVCoords, Normals, Tangents);

	MeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVCoords, TArray<FColor>(), Tangents, true);

	if (AIManager)
	{
		AIManager->GenerateNodes(Vertices, Width, Height);
	}

}

void AProcedurallyGeneratedMap::ClearMap()
{
	Vertices.Empty();
	Triangles.Empty();
	UVCoords.Empty();
	MeshComponent->ClearAllMeshSections();
}

