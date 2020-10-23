// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/Button.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 *
 */
UCLASS()
class ADVGAMESPROGRAMMING_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual bool Initialize() override;

private:

	UPROPERTY(meta = (BindWidget))
		UButton* ButtonHost;

	UPROPERTY(meta = (BindWidget))
		UButton* ButtonJoin;

	UPROPERTY(meta = (BindWidget))
		UButton* ButtonQuit;

	UFUNCTION()
		void OnHostButtonPressed();

	UFUNCTION()
		void OnJoinButtonPressed();

	UFUNCTION()
		void OnQuitButtonPressed();


};
