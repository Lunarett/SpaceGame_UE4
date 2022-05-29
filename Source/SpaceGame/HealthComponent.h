#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UHealthComponent*, HealthComponent, float, Health, float, Damage, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACEGAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Team")
		int TeamNumber;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
		float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
		float MaxHealth;

	virtual void BeginPlay() override;

	UFUNCTION()
		void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	bool IsFriendly(AActor* ActorA, AActor* AActorB);

public:
	void Reset();
	int GetTeamNumber();
	float GetCurrentHealth();

	UFUNCTION(BlueprintCallable)
		void SetTeam(int TeamNum);

	UPROPERTY(BlueprintAssignable, Category = "Health Events")
		FOnHealthChangedSignature OnHealthChanged;
};
