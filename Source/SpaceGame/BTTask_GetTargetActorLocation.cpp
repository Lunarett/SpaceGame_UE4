#include "BTTask_GetTargetActorLocation.h"
#include "GuardAIController.h"
#include "GuardCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTTask_GetTargetActorLocation::UBTTask_GetTargetActorLocation()
{
	NodeName = TEXT("Get Target Location");
}

EBTNodeResult::Type UBTTask_GetTargetActorLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	auto BlackboardComp = OwnerComp.GetBlackboardComponent();
	BlackboardComp->SetValueAsVector(TargetLocationKey.SelectedKeyName, player->GetActorLocation());

	return EBTNodeResult::Succeeded;
}