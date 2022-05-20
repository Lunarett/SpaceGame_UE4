// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveTowardsPlayer.generated.h"

UCLASS()
class SPACEGAME_API UBTTask_MoveTowardsPlayer : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:

	UBTTask_MoveTowardsPlayer();

protected:

	UPROPERTY(EditAnywhere, Category = "BlackBoard")
		struct FBlackboardKeySelector PlayerLocationKey;
};
