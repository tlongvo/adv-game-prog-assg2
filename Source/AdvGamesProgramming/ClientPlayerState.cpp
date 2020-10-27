// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientPlayerState.h"


void AClientPlayerState::BeginPlay()
{
	Super::BeginPlay();
	KillCount = 0;
	DeathCount = 0;
}

void AClientPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AClientPlayerState, DeathCount);
	DOREPLIFETIME(AClientPlayerState, KillCount)
}
