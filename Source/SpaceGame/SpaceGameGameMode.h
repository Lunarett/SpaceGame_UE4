#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceGameGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeamOneWinSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeamTwoWinSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDrawSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSecondSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreChangedSignature);

UCLASS(MinimalAPI)
class ASpaceGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpaceGameGameMode();

protected:
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Teams")
		int TeamOneNumber;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Teams")
		int TeamTwoNumber;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
		int Seconds;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
		int Minutes;

	UPROPERTY(BlueprintReadOnly)
		int CurrentSeconds;

	UPROPERTY(BlueprintReadOnly)
		int CurrentMinutes;

	UPROPERTY(BlueprintReadOnly)
		int TeamOneScore;

	UPROPERTY(BlueprintReadOnly)
		int TeamTwoScore;

	UPROPERTY(BlueprintReadOnly)
		float MatchEndDelay;

	bool bCanCountDown;

	void CalculateScore();
	void UpdateScore();
	void CountDown();
	void CountDownTimerExpired();
	void ResetTimer();

	FTimerHandle TimerHandle_TimeDelay;

public:
	virtual void Tick(float DeltaTime) override;
	void SetTeams();
	void AddKill(int Team);

	int GetTeamOneNumber();
	int GetTeamTwoNumber();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetTeams();

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnTeamOneWinSignature OnTeamOneWin;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnTeamTwoWinSignature OnTeamTwoWin;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnDrawSignature OnDraw;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnScoreChangedSignature OnScoreChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnSecondSignature OnSecond;
};