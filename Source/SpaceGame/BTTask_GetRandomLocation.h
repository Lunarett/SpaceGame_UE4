#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetRandomLocation.generated.h"

UCLASS()
class SPACEGAME_API UBTTask_GetRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UBTTask_GetRandomLocation();

protected:
	UPROPERTY(EditAnywhere, Category = "BlackBoard")
		struct FBlackboardKeySelector RandomWaypointKey;
};
