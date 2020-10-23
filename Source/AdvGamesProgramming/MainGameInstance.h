// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OnlineSubsystem.h"
#include "MainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ADVGAMESPROGRAMMING_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UMainGameInstance(const FObjectInitializer& ObjectInitialize);


private:

	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UMainMenuWidget* Menu;

	IOnlineSubsystem* Subsystem;
	IOnlineSessionPtr SessionInterface;

	void CreateSession(FName SessionName);

	void OnCreateSessionComplete(FName SessionName, bool bSuccess);

public:

	UFUNCTION(BlueprintCallable)
		void LoadMenu();

	void Init();

};
