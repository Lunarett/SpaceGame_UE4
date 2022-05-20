#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GuardAIController.generated.h"

class UAISenseConfig_Sight;
class UBehaviorTreeComponent;
class UBehaviorTree;
class UBlackboardComponent;

UCLASS()
class SPACEGAME_API AGuardAIController : public AAIController
{
	GENERATED_BODY()

public:

	AGuardAIController();

protected:

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual FRotator GetControlRotation() const override;

	UFUNCTION()
		void OnTargetDetected(AActor* SensedActor, FAIStimulus Stimulus);

private:

#pragma region AI Perception Properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Perception Properties", meta = (AllowPrivateAccess = "true"))
		float AISightRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Perception Properties", meta = (AllowPrivateAccess = "true"))
		float AISightAge = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Perception Properties", meta = (AllowPrivateAccess = "true"))
		float AILoseSightRadius = AISightRadius + 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Perception Properties", meta = (AllowPrivateAccess = "true"))
		float AISightFOV = 160.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Perception Properties", meta = (AllowPrivateAccess = "true"))
		float DistanceToPlayer = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Perception Properties", meta = (AllowPrivateAccess = "true"))
		bool bIsPlayerDetected = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI Perception Properties", meta = (AllowPrivateAccess = "true"))
		UAISenseConfig_Sight* SightConfig;

#pragma endregion

	UPROPERTY()
		UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard")
		UBlackboardData* BlackboardData;

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard")
		UBehaviorTree* BehaviorTree;

};
