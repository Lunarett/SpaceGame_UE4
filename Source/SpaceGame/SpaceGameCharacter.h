#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpaceGameCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, int, TeamNum);

class UStaticMesh;
class UCameraComponent;
class USpringArmComponent;
class UHealthComponent;

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
		class USoundBase* FireSound;


	UPROPERTY(ReplicatedUsing = OnRep_SetTeamMaterials)
		int TeamNumber;

	UPROPERTY(Replicated)
		FRotator ShipRotation;

	UFUNCTION()
		void OnRep_SetTeamMaterials();

	void SetTeamMaterials(int TeamNum);

protected:
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* ShipMeshComponent;

	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;

	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(Category = Components, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UHealthComponent* HealthComponent;
		
	UPROPERTY(Replicated)
		float FireForwardValue;

	UPROPERTY(Replicated)
		float FireRightValue;

	UPROPERTY(Replicated, BlueprintReadOnly)
		FVector SpawnPosition;

	UPROPERTY(Replicated)
		bool bCanMove;
	
	bool bDead;

	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void SetShipRotation(FRotator rotation);
	void FireForward(float Value);
	void FireRight(float Value);
	void Fire();
	void FireTimerExpired();
	void OnDied(int TeamNum);
	
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

	UFUNCTION()
		void OnHealthChanged(UHealthComponent* HealthComp, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

private:
	FTimerHandle TimerHandle_ShotTimerExpired;
	uint32 bCanFire : 1;

	void PrintString(FString Msg);

public:
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* TeamOneMaterial;

	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* TeamTwoMaterial;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnDeathSignature OnDeath;

	void Respawn();
	void ApplyTeamMaterials(int TeamNum);
	void AllowControl(bool bAllow);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAllowControl(bool bAllow);

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
