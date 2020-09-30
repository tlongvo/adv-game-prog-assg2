// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "AIManager.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProcedurallyGeneratedMap.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API AProcedurallyGeneratedMap : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProcedurallyGeneratedMap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;

	UPROPERTY(VisibleAnywhere)
		UProceduralMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
		int32 Width;

	UPROPERTY(EditAnywhere)
		int32 Height;

	UPROPERTY(EditAnywhere)
		float GridSize;

	UPROPERTY(VisibleAnywhere)
		TArray<FVector> Vertices;

	UPROPERTY(VisibleAnywhere)
		TArray<int32> Triangles;

	UPROPERTY(VisibleAnywhere)
		TArray<FVector2D> UVCoords;

	UPROPERTY(VisibleAnywhere)
		TArray<FVector> Normals;

	UPROPERTY(VisibleAnywhere)
		TArray<FProcMeshTangent> Tangents;

	UPROPERTY(EditAnywhere)
		float PerlinScale;

	UPROPERTY(EditAnywhere)
		float PerlinRoughness;

	UFUNCTION(BlueprintCallable)
		void GenerateMap();

	void ClearMap();

	UPROPERTY(EditAnywhere)
		bool bRegenerateMap;

	UPROPERTY(EditAnywhere)
		AAIManager* AIManager;

};
