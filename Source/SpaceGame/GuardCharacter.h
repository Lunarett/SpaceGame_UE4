// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "SpaceGameCharacter.h"
#include "GuardCharacter.generated.h"

UCLASS()
class SPACEGAME_API AGuardCharacter : public ASpaceGameCharacter
{
	GENERATED_BODY()
	
public:
	AGuardCharacter();

	bool bLateStart;

	UPROPERTY(Replicated)
	bool bAIStartFire;

	FTimerHandle AITimerHandle_ShotTimerExpired;
	FTimerHandle TimerHandle_Delay;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void AIFire();
	void AIFireTimerExpired();
	void TimerExpired();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAIFire();
};