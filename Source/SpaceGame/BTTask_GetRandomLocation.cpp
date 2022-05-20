#include "BTTask_GetRandomLocation.h"
#include "GuardAIController.h"
#include "GuardCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"


UBTTask_GetRandomLocation::UBTTask_GetRandomLocation()
{
	NodeName = TEXT("Get Random Location");
}

EBTNodeResult::Type UBTTask_GetRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	FVector RandomLocation;

	bool bS = NavSystem->K2_GetRandomReachablePointInRadius(GetWorld(), ControlledPawn->GetActorLocation(), RandomLocation, 600);

	if (bS)
	{
		auto BlackboardComp = OwnerComp.GetBlackboardComponent();
		BlackboardComp->SetValueAsVector(RandomWaypointKey.SelectedKeyName, RandomLocation);
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}