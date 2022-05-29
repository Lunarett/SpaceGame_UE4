#include "GuardCharacter.h"
#include "GuardAiController.h"
#include "SpaceGameProjectile.h"
#include "HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

AGuardCharacter::AGuardCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);

	AIControllerClass = AGuardAIController::StaticClass();

	SetReplicates(true);
}

void AGuardCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetTeamMaterials(HealthComponent->GetTeamNumber());
}

void AGuardCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bAIStartFire)
	{
		AIFire();
	}
}

void AGuardCharacter::AIFire()
{
	if (!HasAuthority())
	{
		ServerAIFire();
	}

	FVector Direction = GetShipMeshComponent()->GetForwardVector();

	if (bCanFire && bCanMove && Projectiles.Num() > 0)
	{
		if (Direction.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = Direction.Rotation();
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();

			if (World != nullptr && Projectiles.Num() > 0)
			{
				// Spawn the projectile
				if (NextProjectile == InitialProjectileSpawnCount)
				{
					NextProjectile = 0;
				}

				Projectiles[NextProjectile]->GetProjectileMovement()->SetUpdatedComponent(Projectiles[NextProjectile]->GetRootComponent());

				Projectiles[NextProjectile]->SetActorLocation(SpawnLocation);
				Projectiles[NextProjectile]->SetActorScale3D(FVector(2, 1, 1));
				Projectiles[NextProjectile]->GetProjectileMovement()->Velocity = Direction * Projectiles[NextProjectile]->GetProjectileMovement()->InitialSpeed;

				NextProjectile++;
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(AITimerHandle_ShotTimerExpired, this, &AGuardCharacter::AIFireTimerExpired, FireRate);

			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void AGuardCharacter::AIFireTimerExpired()
{
	bCanFire = true;
}

void AGuardCharacter::TimerExpired()
{
	bLateStart = true;
}

void AGuardCharacter::ServerAIFire_Implementation()
{
	AIFire();
}

bool AGuardCharacter::ServerAIFire_Validate()
{
	return true;
}

void AGuardCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGuardCharacter, bAIStartFire);
}