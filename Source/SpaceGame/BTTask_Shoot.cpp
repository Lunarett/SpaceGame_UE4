#include "BTTask_Shoot.h"
#include "GuardAIController.h"
#include "GuardCharacter.h"

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	AGuardCharacter* GuardCharacter = Cast<AGuardCharacter>(ControlledPawn);

	if (GuardCharacter)
	{
		GuardCharacter->bAIStartFire = true;
	}

	return EBTNodeResult::Succeeded;
}