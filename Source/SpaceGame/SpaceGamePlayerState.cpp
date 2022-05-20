#include "SpaceGamePlayerState.h"
#include "Net/UnrealNetwork.h"

void ASpaceGamePlayerState::AddKill()
{
	Kills++;
}

void ASpaceGamePlayerState::AddDeath()
{
	Deaths++;
}

void ASpaceGamePlayerState::SetTeam(int TeamNumber)
{
	Team = TeamNumber;
}

void ASpaceGamePlayerState::Reset()
{
	Kills = 0;
	Deaths = 0;
}

int ASpaceGamePlayerState::GetKills()
{
	return Kills;
}

int ASpaceGamePlayerState::GetDeaths()
{
	return Deaths;
}

int ASpaceGamePlayerState::GetTeamNumber()
{
	return Team;
}

void ASpaceGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpaceGamePlayerState, Kills);
	DOREPLIFETIME(ASpaceGamePlayerState, Deaths);
	DOREPLIFETIME(ASpaceGamePlayerState, Team);
}