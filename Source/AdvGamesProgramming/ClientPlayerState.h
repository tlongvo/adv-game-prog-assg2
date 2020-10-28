// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "ClientPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ADVGAMESPROGRAMMING_API AClientPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadWrite)
	int32 KillCount;
	UPROPERTY(Replicated)
	int32 DeathCount; 
};
