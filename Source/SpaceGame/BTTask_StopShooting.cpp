#include "BTTask_StopShooting.h"
#include "GuardAIController.h"
#include "GuardCharacter.h"

EBTNodeResult::Type UBTTask_StopShooting::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	AGuardCharacter* GuardCharacter = Cast<AGuardCharacter>(ControlledPawn);

	if (GuardCharacter)
	{
		GuardCharacter->bAIStartFire = false;
	}

	return EBTNodeResult::Succeeded;
}