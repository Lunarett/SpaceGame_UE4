#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetTargetActorLocation.generated.h"

UCLASS()
class SPACEGAME_API UBTTask_GetTargetActorLocation : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UBTTask_GetTargetActorLocation();

protected:
	UPROPERTY(EditAnywhere, Category = "BlackBoard")
		struct FBlackboardKeySelector TargetLocationKey;
};
