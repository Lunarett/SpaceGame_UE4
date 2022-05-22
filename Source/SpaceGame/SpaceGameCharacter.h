#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpaceGameCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDeathSignature, ASpaceGameCharacter*, KilledCharacter, AController*, InstigatedBy, AActor*, DamageCauser);

class USoundBase;
class UStaticMesh;
class UCameraComponent;
class USpringArmComponent;
class UHealthComponent;
class ASpaceGameProjectile;

UCLASS()
class SPACEGAME_API ASpaceGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpaceGameCharacter();

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		FVector GunOffset;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float FireRate;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float MoveSpeed;

	UPROPERTY(Replicated, Category = Audio, EditAnywhere, BlueprintReadWrite)
		USoundBase* FireSound;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
		bool IsAI;

	UPROPERTY(ReplicatedUsing = OnRep_SetTeamMaterials, BlueprintReadWrite)
		int TeamNumber;

	UPROPERTY(Replicated)
		FRotator ShipRotation;

	UFUNCTION()
		void OnRep_SetTeamMaterials();

	UFUNCTION(BlueprintCallable)
		void SetTeamMaterials(int TeamNum);


public:
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* ShipMeshComponent;

	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* CameraComponent;

	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	UPROPERTY(Category = Components, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UHealthComponent* HealthComponent;

	UPROPERTY(Replicated, BlueprintReadWrite)
		float FireForwardValue;

	UPROPERTY(Replicated)
		float FireRightValue;

	UPROPERTY(Replicated, BlueprintReadOnly)
		FVector SpawnPosition;

	UPROPERTY(Replicated)
		bool bCanMove;

	//Projectile Object Pool
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Object Pool")
		int InitialProjectileSpawnCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Object Pool")
		FVector ObjectPoolLocation;

	UPROPERTY(Replicated)
		TArray<ASpaceGameProjectile*> Projectiles;

	int NextProjectile = 0;

	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void SetShipRotation(FRotator rotation);
	void FireForward(float Value);
	void FireRight(float Value);
	void Fire();
	void FireTimerExpired();
	void OnDied(int TeamNum);
	void SpawnProjectiles();
	void ApplyTeamMaterials(int TeamNum);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetShipRotation(FRotator rotation);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFireForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFireRight(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRespawn();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnProjectiles();


	UFUNCTION()
		void OnHealthChanged(UHealthComponent* HealthComp, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	uint32 bCanFire : 1;

private:
	FTimerHandle TimerHandle_ShotTimerExpired;


	void PrintString(FString Msg);

public:
	FORCEINLINE UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* TeamOneMaterial;

	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* TeamTwoMaterial;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnDeathSignature OnDeath;

	UPROPERTY(BlueprintReadOnly)
		TArray<AActor*> AllPlayersArray;

	void Respawn();
	void AllowControl(bool bAllow);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAllowControl(bool bAllow);

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
