#include "SpaceGameGameMode.h"
#include "SpaceGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "HealthComponent.h"
#include "GameFramework/PlayerStart.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

ASpaceGameGameMode::ASpaceGameGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = ASpaceGameCharacter::StaticClass();

	SetReplicates(true);

	OnTeamOneWin.AddDynamic(this, &ASpaceGameGameMode::CalculateScore);
	OnTeamTwoWin.AddDynamic(this, &ASpaceGameGameMode::CalculateScore);
	OnDraw.AddDynamic(this, &ASpaceGameGameMode::CalculateScore);
	OnScoreChanged.AddDynamic(this, &ASpaceGameGameMode::UpdateScore);
	OnSecond.AddDynamic(this, &ASpaceGameGameMode::CountDown);

	bCanCountDown = true;

	TeamOneNumber = 0;
	TeamTwoNumber = 1;

	Minutes = 1;
	Seconds = 30;
	MatchEndDelay = 5.0f;

	CurrentMinutes = Minutes;
	CurrentSeconds = Seconds;
}

void ASpaceGameGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CountDown();
}

void ASpaceGameGameMode::CalculateScore()
{
	UWorld* World = GetWorld();
	TArray<AActor*> Characters;
	ASpaceGameCharacter* Character = nullptr;

	UGameplayStatics::GetAllActorsOfClass(this, ASpaceGameCharacter::StaticClass(), Characters);

	for (int i = 0; i < Characters.Num(); i++)
	{
		Character = Cast<ASpaceGameCharacter>(Characters[i]);

		if (Character)
		{
			Character->AllowControl(false);
		}
	}

	if (TeamOneScore > TeamTwoScore)
	{
		OnTeamOneWin.Broadcast();
	}
	
	if (TeamTwoScore > TeamOneScore)
	{
		OnTeamTwoWin.Broadcast();
	}
	
	if (TeamTwoScore == TeamOneScore)
	{
		OnDraw.Broadcast();
	}
}

void ASpaceGameGameMode::UpdateScore()
{
	OnScoreChanged.Broadcast();
}

void ASpaceGameGameMode::CountDown()
{
	if (bCanCountDown)
	{
		if (CurrentSeconds <= 0 && CurrentMinutes <= 0)
		{
			CalculateScore();
			GetWorldTimerManager().SetTimer(TimerHandle_TimeDelay, this, &ASpaceGameGameMode::ResetTimer, MatchEndDelay, false);
			bCanCountDown = false;
			return;
		}

		CurrentSeconds = FMath::Clamp(CurrentSeconds - 1, 0, 59);

		if (CurrentSeconds <= 0 && CurrentMinutes > 0)
		{
			CurrentMinutes = CurrentSeconds = FMath::Clamp(CurrentMinutes - 1, 0, 59);
			CurrentSeconds = 59;
		}


		OnSecond.Broadcast();
		GetWorldTimerManager().SetTimer(TimerHandle_TimeDelay, this, &ASpaceGameGameMode::CountDownTimerExpired, 1.0f, false);
	}

	bCanCountDown = false;
}

void ASpaceGameGameMode::CountDownTimerExpired()
{
	bCanCountDown = true;
}

void ASpaceGameGameMode::ResetTimer()
{
	UWorld* World = GetWorld();
	TArray<AActor*> Characters;
	ASpaceGameCharacter* Character = nullptr;

	UGameplayStatics::GetAllActorsOfClass(this, ASpaceGameCharacter::StaticClass(), Characters);

	for (int i = 0; i < Characters.Num(); i++)
	{
		Character = Cast<ASpaceGameCharacter>(Characters[i]);
		
		if (Character)
		{
			Character->Respawn();
			Character->AllowControl(true);
		}
	}

	TeamOneScore = 0;
	TeamTwoScore = 0;
	UpdateScore();

	CurrentSeconds = Seconds;
	CurrentMinutes = Minutes;
	bCanCountDown = true;
}

void ASpaceGameGameMode::SetTeams()
{
	if (!HasAuthority())
	{
		ServerSetTeams();
		return;
	}

	UWorld* World = GetWorld();
	TArray<AActor*> Characters;

	UGameplayStatics::GetAllActorsOfClass(this, ASpaceGameCharacter::StaticClass(), Characters);

	ASpaceGameCharacter* Character = nullptr;

	for (int i = 0; i < Characters.Num(); i++)
	{
		Character = Cast<ASpaceGameCharacter>(Characters[i]);
		// Assign team number to every character
		if (Character)
		{
			if (i % 2 == 0)
			{
				Character->GetHealthComponent()->SetTeam(TeamTwoNumber);
				Character->TeamNumber = TeamTwoNumber;
				Character->SetTeamMaterials(TeamTwoNumber);
			}
			else
			{
				Character->GetHealthComponent()->SetTeam(TeamOneNumber);
				Character->TeamNumber = TeamOneNumber;
				Character->SetTeamMaterials(TeamOneNumber);
			}
		}
	}
}

void ASpaceGameGameMode::AddKill(int Team)
{


	if (Team == TeamOneNumber)
	{
		TeamOneScore++;
	}
	else if (Team == TeamTwoNumber)
	{
		TeamTwoScore++;
	}

	OnScoreChanged.Broadcast();
}

int ASpaceGameGameMode::GetTeamOneNumber()
{
	return TeamOneNumber;
}

int ASpaceGameGameMode::GetTeamTwoNumber()
{
	return TeamTwoNumber;
}

void ASpaceGameGameMode::ServerSetTeams_Implementation()
{
	SetTeams();
}

bool ASpaceGameGameMode::ServerSetTeams_Validate()
{
	return true;
}

void ASpaceGameGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpaceGameGameMode, TeamOneNumber);
	DOREPLIFETIME(ASpaceGameGameMode, TeamTwoNumber);
}
