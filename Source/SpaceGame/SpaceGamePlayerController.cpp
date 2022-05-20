#include "SpaceGamePlayerController.h"
#include "SpaceGameCharacter.h"

ASpaceGamePlayerController::ASpaceGamePlayerController()
{
	bAutoManageActiveCameraTarget = false;
	bFindCameraComponentWhenViewTarget = true;
}

void ASpaceGamePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ASpaceGameCharacter* character = Cast<ASpaceGameCharacter>(InPawn);

	if (character)
	{
		SetViewTarget(InPawn);
	}

	if (HasAuthority())
	{
		OnPossessWithAuth.Broadcast(this, InPawn);
	}
}

void ASpaceGamePlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (HasAuthority())
	{
		OnUnPossessWithAuth.Broadcast(this);
	}
}
