#include "GuardAIController.h"
#include "GuardCharacter.h"
#include "SpaceGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Math/NumericLimits.h"


AGuardAIController::AGuardAIController()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI Sight Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component")));

	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AISightFOV;
	SightConfig->SetMaxAge(AISightAge);

	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AGuardAIController::OnTargetDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void AGuardAIController::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(BlackboardData)) { return; }
	UseBlackboard(BlackboardData, BlackboardComponent);

	if (!ensure(BehaviorTree)) { return; }
	RunBehaviorTree(BehaviorTree);
}

void AGuardAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

FRotator AGuardAIController::GetControlRotation() const
{
	if (GetPawn() == nullptr) { return FRotator::ZeroRotator; }

	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);
}

void AGuardAIController::OnTargetDetected(AActor* SensedActor, FAIStimulus Stimulus)
{
	BlackboardComponent->SetValueAsBool("CanSeePlayer", Stimulus.WasSuccessfullySensed());
	BlackboardComponent->SetValueAsObject("SelfActor", SensedActor);

	if (Stimulus.WasSuccessfullySensed())
	{
		ASpaceGameCharacter* SensedCharacter = Cast<ASpaceGameCharacter>(SensedActor);
		ASpaceGameCharacter* ControlledCharacter = Cast<ASpaceGameCharacter>(GetCharacter());

		if (SensedCharacter && ControlledCharacter)
		{
			bool IsFriendly = SensedCharacter->TeamNumber == ControlledCharacter->TeamNumber;
			BlackboardComponent->SetValueAsBool("IsFriendly", IsFriendly);
		}
	}
}
