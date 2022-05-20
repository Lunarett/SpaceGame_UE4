#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SpaceGamePlayerState.generated.h"

UCLASS()
class SPACEGAME_API ASpaceGamePlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	UPROPERTY(Replicated, BlueprintReadOnly)
		int Kills;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int Deaths;

	UPROPERTY(Replicated, BlueprintReadOnly)
		int Team;

public:
	void AddKill();
	void AddDeath();
	void SetTeam(int TeamNumber);
	void Reset();

	int GetKills();
	int GetDeaths();
	int GetTeamNumber();
};
