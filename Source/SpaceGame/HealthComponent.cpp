#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	MaxHealth = 100;
	CurrentHealth = MaxHealth;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();

		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
		}
	}
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f) { return; }
	if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser)) { return; }

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}

bool UHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{

	if (ActorA == nullptr || ActorB == nullptr) { return true; }

	UHealthComponent* HealthComponentA = Cast<UHealthComponent>(ActorA->GetComponentByClass(UHealthComponent::StaticClass()));
	UHealthComponent* HealthComponentB = Cast<UHealthComponent>(ActorB->GetComponentByClass(UHealthComponent::StaticClass()));

	if (HealthComponentA == nullptr || HealthComponentB == nullptr) { return true; }

	return HealthComponentA->TeamNumber == HealthComponentB->TeamNumber;
}

void UHealthComponent::Reset()
{
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(this, CurrentHealth, 0, nullptr, nullptr, nullptr);
}

int UHealthComponent::GetTeamNumber()
{
	return TeamNumber;
}

float UHealthComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

void UHealthComponent::SetTeam(int TeamNum)
{
	TeamNumber = TeamNum;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}
