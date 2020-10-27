// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "HealthComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	UHealthComponent* HealthComponent;
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ENetRole> LocalRole;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float NormalMovementSpeed;
	float SprintMovementSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void Strafe(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	void SprintStart();
	void SprintEnd();


	void OnDeath();

	/**
	 * Will adjust the movement speed of the server character to sprinting
	 */
	UFUNCTION(Server, Reliable)
		void ServerSprintStart();

	/**
	 * Will adjust the movement speed of the server character to normal walking speed
	 */
	UFUNCTION(Server, Reliable)
		void ServerSprintEnd();

	 /**
	  * Client function that will hide or show the hud.
	  * @param bSetHudVisibility: Whether the hud should be hidden or shown.
	  */
	UFUNCTION(Client, Reliable)
		void HidePlayerHUD(bool bSetHUDVisibility);

	//Update Text relevant to PlayerState here.
	UFUNCTION(Client, Reliable)
		void UpdatePlayerHUD();

	void IncreaseSpeed(float SpeedMultiplier);
	//Increase movement speed by specified amount
	UFUNCTION(Server, Reliable)
		void ServerIncreaseSpeed(float SpeedMultiplier);


private:
	UPROPERTY(EditInstanceOnly)
	float LookSensitivity;

	UPROPERTY(EditInstanceOnly)
	float SprintMultiplier;

	UCameraComponent* Camera;
};
