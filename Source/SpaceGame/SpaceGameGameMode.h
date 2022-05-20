#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceGameGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeamOneWinSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeamTwoWinSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDrawSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSecondSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreChangedSignature);

class ASpaceGameCharacter;
class ASpaceGamePlayerController;

UCLASS(MinimalAPI)
class ASpaceGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASpaceGameGameMode();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Teams")
		int TeamOneNumber = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Teams")
		int TeamTwoNumber = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
		int Seconds = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
		int Minutes = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
		float MatchEndDelay = 5.0f;
	
	UPROPERTY(BlueprintReadOnly)
		int CurrentSeconds;

	UPROPERTY(BlueprintReadOnly)
		int CurrentMinutes;

	UPROPERTY(BlueprintReadOnly)
		int TeamOneScore;

	UPROPERTY(BlueprintReadOnly)
		int TeamTwoScore;

	int CurrentPlayerCount = 0;
	bool bCanCountDown = false;

	virtual void BeginPlay() override;
	void CalculateScore();
	void UpdateScore();
	void CountDown();
	void CountDownTimerExpired();
	void ResetTimer();
	void RespawnPlayer(ASpaceGamePlayerController* PlayerController, int TeamNum);

	UFUNCTION()
		void PossessWithAuth(ASpaceGamePlayerController* PlayerController, APawn* TargetPawn);

	UFUNCTION()
		void OnPlayerDeath(ASpaceGameCharacter* Character, AController* InstigatedBy, AActor* DamageCauser);
	
	FTimerHandle TimerHandle_TimeDelay;

public:

	virtual void Tick(float DeltaTime) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void SetTeams();
	void AddTeamPoints(int Team);


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