#include "GuardCharacter.h"
#include "GuardAiController.h"
#include "GameFramework/CharacterMovementComponent.h"

AGuardCharacter::AGuardCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);

	AIControllerClass = AGuardAIController::StaticClass();
}