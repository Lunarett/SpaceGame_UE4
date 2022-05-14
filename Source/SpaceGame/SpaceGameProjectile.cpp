// Copyright Epic Games, Inc. All Rights Reserve

#include "SpaceGameProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ASpaceGameProjectile::ASpaceGameProjectile()
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ASpaceGameProjectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	Damage = 10;

	SetReplicates(true);
}

void ASpaceGameProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	InitialLocation = GetActorLocation();
	ProjectileMovement->SetUpdatedComponent(nullptr);
}

void ASpaceGameProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* MyOwner = GetOwner();

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
	}

	if (MyOwner != nullptr && OtherActor != nullptr)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetOwner()->GetInstigatorController(), GetOwner(), DamageType);
	}
	else
	{
		if (MyOwner == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Owner null");
		}

		if (OtherActor == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Other null");
		}
	}
	
	RemoveProjectile();
}

void ASpaceGameProjectile::SpawnInProjectile(FVector Location, FVector Direction)
{
	if (!HasAuthority())
	{
		ServerSpawnInProjectile(Location, Direction);
		return;
	}

	ProjectileMovement->SetUpdatedComponent(GetRootComponent());
	SetActorLocation(Location);
	ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
}

void ASpaceGameProjectile::RemoveProjectile()
{
	if (!HasAuthority())
	{
		ServerRemoveProjectile();
		return;
	}

	SetActorLocation(InitialLocation);
	ProjectileMovement->SetUpdatedComponent(nullptr);
}

void ASpaceGameProjectile::ServerSpawnInProjectile_Implementation(FVector Location, FVector Direction)
{
	SpawnInProjectile(Location, Direction);
}

bool ASpaceGameProjectile::ServerSpawnInProjectile_Validate(FVector Location, FVector Direction)
{
	return true;
}

void ASpaceGameProjectile::ServerRemoveProjectile_Implementation()
{
	RemoveProjectile();
}

bool ASpaceGameProjectile::ServerRemoveProjectile_Validate()
{
	return true;
}
