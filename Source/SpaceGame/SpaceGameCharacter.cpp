#include "SpaceGameCharacter.h"
#include "SpaceGameProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "HealthComponent.h"
#include "SpaceGameGameMode.h"
#include "Net/UnrealNetwork.h"


ASpaceGameCharacter::ASpaceGameCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the mesh component
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	//RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetupAttachment(RootComponent);
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);

	// Cache sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(ShipMeshComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when ship does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->SetRelativeRotation(FRotator(-80.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	MoveSpeed = 1000.0f;
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
	bCanMove = true;

	InitialProjectileSpawnCount = 20;

	SetReplicates(true);
	ShipMeshComponent->SetIsReplicated(true);
}

void ASpaceGameCharacter::OnRep_SetTeamMaterials()
{
	this->SetTeamMaterials(TeamNumber);
}

void ASpaceGameCharacter::SetTeamMaterials(int TeamNum)
{
	if (TeamNum == 0)
	{
		ShipMeshComponent->SetMaterial(0, TeamOneMaterial);
	}
	else
	{
		ShipMeshComponent->SetMaterial(0, TeamTwoMaterial);
	}
}

void ASpaceGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	ASpaceGameGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceGameGameMode>();
	SpawnProjectiles();

	if (HasAuthority())
	{
		GM->SetTeams();
		SpawnPosition = GetActorLocation();
		HealthComponent->OnHealthChanged.AddDynamic(this, &ASpaceGameCharacter::OnHealthChanged);
		OnDeath.AddDynamic(this, &ASpaceGameCharacter::OnDied);
	}

}

void ASpaceGameCharacter::AllowControl(bool bAllow)
{
	if (!HasAuthority())
	{
		ServerAllowControl(bAllow);
		return;
	}

	bCanMove = bAllow;
}

void ASpaceGameCharacter::ServerAllowControl_Implementation(bool bAllow)
{
	AllowControl(bAllow);
}

bool ASpaceGameCharacter::ServerAllowControl_Validate(bool bAllow)
{
	return true;
}

void ASpaceGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Fire();
}

void ASpaceGameCharacter::MoveForward(float Value)
{
	if (Value != 0.0f && bCanMove)
	{
		AddMovementInput(GetActorForwardVector() * Value);

		FVector Direction = GetActorForwardVector() * 180.0f * Value;

		ShipRotation = Direction.Rotation();
		SetShipRotation(ShipRotation);
	}
}

void ASpaceGameCharacter::MoveRight(float Value)
{
	if (Value != 0.0f && bCanMove)
	{
		AddMovementInput(GetActorRightVector() * Value);

		ShipRotation = GetActorRightVector().Rotation() * Value;
		SetShipRotation(ShipRotation);
	}
}

void ASpaceGameCharacter::SetShipRotation(FRotator rotation)
{
	if (!HasAuthority())
	{ 
		ServerSetShipRotation(rotation);
		return;
	}

	ShipMeshComponent->SetRelativeRotation(rotation);
}

void ASpaceGameCharacter::FireForward(float Value)
{
	if (!HasAuthority())
	{
		ServerFireForward(Value);
	}

	FireForwardValue = Value;
}

void ASpaceGameCharacter::FireRight(float Value)
{
	if (!HasAuthority())
	{
		ServerFireRight(Value);
	}

	FireRightValue = Value;
}

void ASpaceGameCharacter::Fire()
{
	if (!HasAuthority())
	{
		ServerFire();
	}

	const FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);

	if (bCanFire && bCanMove)
	{
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();

			if (World != nullptr && Projectiles.Num() > 0)
			{
				// Spawn the projectile
				if (NextProjectile == InitialProjectileSpawnCount)
				{
					NextProjectile = 0;
				}

				Projectiles[NextProjectile]->SetActorScale3D(FVector(2, 1, 1));
				Projectiles[NextProjectile]->SpawnInProjectile(SpawnLocation, FireDirection);

				NextProjectile++;
			}
			else
			{
				PrintString("NULL");
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ASpaceGameCharacter::FireTimerExpired, FireRate);

			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void ASpaceGameCharacter::FireTimerExpired()
{
	bCanFire = true;
}

void ASpaceGameCharacter::Respawn()
{
	if (!HasAuthority())
	{
		ServerRespawn();
		return;
	}

	HealthComponent->Reset();
	SetActorLocation(SpawnPosition);
}

void ASpaceGameCharacter::OnDied(int TeamNum)
{
	OnDeath.Broadcast(TeamNum);

	ASpaceGameGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceGameGameMode>();

	if (GM)
	{
		GM->AddKill(TeamNum);
	}
}

void ASpaceGameCharacter::SpawnProjectiles()
{
	if (!HasAuthority())
	{
		ServerSpawnProjectiles();
		return;
	}

	FRotator rot = FRotator::ZeroRotator;

	for (int i = 0; i < InitialProjectileSpawnCount; i++)
	{
		ObjectPoolLocation = FVector((i * 100), 0, 0);
		
		ASpaceGameProjectile* projectile = GetWorld()->SpawnActor<ASpaceGameProjectile>(ObjectPoolLocation, rot);

		if (projectile)
		{
			projectile->SetOwner(this);
			Projectiles.Add(projectile);
		}
	}
}

#pragma region Server
void ASpaceGameCharacter::ServerSetShipRotation_Implementation(FRotator rotation)
{
	SetShipRotation(rotation);
}

bool ASpaceGameCharacter::ServerSetShipRotation_Validate(FRotator rotation)
{
	return true;
}

void ASpaceGameCharacter::ServerFireForward_Implementation(float Value)
{
	FireForward(Value);
}

bool ASpaceGameCharacter::ServerFireForward_Validate(float Value)
{
	return true;
}

void ASpaceGameCharacter::ServerFireRight_Implementation(float Value)
{
	FireRight(Value);
}

bool ASpaceGameCharacter::ServerFireRight_Validate(float Value)
{
	return true;
}

void ASpaceGameCharacter::ServerFire_Implementation()
{
	Fire();
}

bool ASpaceGameCharacter::ServerFire_Validate()
{
	return true;
}

void ASpaceGameCharacter::ServerRespawn_Implementation()
{
	Respawn();
}

bool ASpaceGameCharacter::ServerRespawn_Validate()
{
	return true;
}

void ASpaceGameCharacter::ServerSpawnProjectiles_Implementation()
{
	SpawnProjectiles();
}

bool ASpaceGameCharacter::ServerSpawnProjectiles_Validate()
{
	return true;
}

#pragma endregion

void ASpaceGameCharacter::OnHealthChanged(UHealthComponent* HealthComp, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f)
	{

		ASpaceGameCharacter* Causer = Cast<ASpaceGameCharacter>(InstigatedBy->GetCharacter());

		if (Causer)
		{
			OnDied(Causer->HealthComponent->GetTeamNumber());
		}

		Respawn();
	}
}

void ASpaceGameCharacter::PrintString(FString Msg)
{
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Cyan, Msg);
}

void ASpaceGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASpaceGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpaceGameCharacter::MoveRight);

	PlayerInputComponent->BindAxis("FireForward", this, &ASpaceGameCharacter::FireForward);
	PlayerInputComponent->BindAxis("FireRight", this, &ASpaceGameCharacter::FireRight);
}

void ASpaceGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpaceGameCharacter, FireForwardValue);
	DOREPLIFETIME(ASpaceGameCharacter, FireRightValue);
	DOREPLIFETIME(ASpaceGameCharacter, ShipRotation);
	DOREPLIFETIME(ASpaceGameCharacter, SpawnPosition);
	DOREPLIFETIME(ASpaceGameCharacter, TeamNumber);
	DOREPLIFETIME(ASpaceGameCharacter, bCanMove);
	DOREPLIFETIME(ASpaceGameCharacter, FireSound);
	DOREPLIFETIME(ASpaceGameCharacter, Projectiles);
}

