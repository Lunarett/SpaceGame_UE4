#include "SpaceGameGameMode.h"
#include "SpaceGameCharacter.h"
#include "SpaceGamePlayerState.h"
#include "SpaceGamePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "HealthComponent.h"
#include "GameFramework/PlayerStart.h"
#include "TimerManager.h"

ASpaceGameGameMode::ASpaceGameGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = ASpaceGameCharacter::StaticClass();
	PlayerControllerClass = ASpaceGamePlayerController::StaticClass();
	PlayerStateClass = ASpaceGamePlayerState::StaticClass();
}

void ASpaceGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ASpaceGamePlayerController* PlayerController = Cast<ASpaceGamePlayerController>(NewPlayer);
	ASpaceGameCharacter* Character = nullptr;

	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Yellow, "Player Logged In");
	
	if (PlayerController)
	{
		PlayerController->OnPossessWithAuth.AddDynamic(this, &ASpaceGameGameMode::PossessWithAuth);

		Character = Cast<ASpaceGameCharacter>(PlayerController->GetPawn());

		if (Character)
		{
			Character->OnDeath.AddDynamic(this, &ASpaceGameGameMode::OnPlayerDeath);
		}

		CurrentPlayerCount++;

		// Run when all players have joined
		if (CurrentPlayerCount >= 4)
		{
			GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, "Running Set Teams");
			SetTeams();
			bCanCountDown = true;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, "Player Controller is Null");
	}
}

void ASpaceGameGameMode::BeginPlay()
{
	Super::BeginPlay();

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

void ASpaceGameGameMode::OnPlayerDeath(ASpaceGameCharacter* Character, AController* InstigatedBy, AActor* DamageCauser)
{
	ASpaceGamePlayerController* PlayerController = Cast<ASpaceGamePlayerController>(Character->Controller);

	ASpaceGamePlayerState* CharacterPlayerState = Cast<ASpaceGamePlayerState>(Character->GetPlayerState());
	ASpaceGamePlayerState* KillerPlayerState = nullptr;

	if (InstigatedBy)
	{
		KillerPlayerState = Cast<ASpaceGamePlayerState>(InstigatedBy->PlayerState);
	}

	// Respawn Killed player
	if (PlayerController && CharacterPlayerState)
	{
		Character->Respawn();
		//RespawnPlayer(PlayerController, CharacterPlayerState->GetTeamNumber());
		CharacterPlayerState->AddDeath();
	}

	//Give point to Killers team
	if (KillerPlayerState)
	{
		KillerPlayerState->AddKill();
		AddTeamPoints(KillerPlayerState->GetTeamNumber());
	}
}

void ASpaceGameGameMode::RespawnPlayer(ASpaceGamePlayerController* PlayerController, int TeamNum)
{
	UWorld* World = GetWorld();
	APlayerStart* PlayerStart = nullptr;
	TArray<AActor*> PlayerStarts;

	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	if (PlayerStarts.Num() > 0)
	{
		PlayerStart = Cast<APlayerStart>(PlayerStarts[FMath::RandRange(0, PlayerStarts.Num())]);

		UClass* PawnClass = GetDefaultPawnClassForController(PlayerController);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APawn* SpawnedPlayer = GetWorld()->SpawnActor<APawn>(PawnClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParams);
		PlayerController->Possess(SpawnedPlayer);

		ASpaceGameCharacter* Character = Cast<ASpaceGameCharacter>(SpawnedPlayer);

		if (Character)
		{
			Character->SetTeamMaterials(TeamNum);
		}

		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, "Respawned Player");
	}
}

void ASpaceGameGameMode::PossessWithAuth(ASpaceGamePlayerController* PlayerController, APawn* TargetPawn)
{
	ASpaceGameCharacter* Character = Cast<ASpaceGameCharacter>(TargetPawn);
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Yellow, "Possesed");

	if (Character)
	{
		Character->OnDeath.AddDynamic(this, &ASpaceGameGameMode::OnPlayerDeath);
	}
}

void ASpaceGameGameMode::SetTeams()
{
	UWorld* World = GetWorld();
	TArray<AActor*> Characters;
	ASpaceGameCharacter* Character = nullptr;
	ASpaceGamePlayerState* PlayerState = nullptr;

	UGameplayStatics::GetAllActorsOfClass(this, ASpaceGameCharacter::StaticClass(), Characters);


	for (int i = 0; i < Characters.Num(); i++)
	{
		Character = Cast<ASpaceGameCharacter>(Characters[i]);
		
		// Assign team number to every character
		if (Character)
		{
			if (Character->IsAI)
			{
				continue;
			}

			if (i % 2 == 0)
			{
				Character->GetHealthComponent()->SetTeam(TeamTwoNumber);
				Character->TeamNumber = TeamTwoNumber;
				Character->SetTeamMaterials(TeamTwoNumber);

				if (PlayerState) { PlayerState->SetTeam(TeamTwoNumber); }
			}
			else
			{
				Character->GetHealthComponent()->SetTeam(TeamOneNumber);
				Character->TeamNumber = TeamOneNumber;
				Character->SetTeamMaterials(TeamOneNumber);

				if (PlayerState) { PlayerState->SetTeam(TeamOneNumber); }
			}
		}
	}
}

void ASpaceGameGameMode::AddTeamPoints(int Team)
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
