#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpaceGamePlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPossesedSignature, class ASpaceGamePlayerController*, PlayerController, APawn*, TargetPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnPossesedSignature, class ASpaceGamePlayerController*, PlayerController);

UCLASS()
class SPACEGAME_API ASpaceGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASpaceGamePlayerController();

protected:
	void OnPossess(APawn* InPawn) override;
	void OnUnPossess() override;

public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnPossesedSignature OnPossessWithAuth;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnUnPossesedSignature OnUnPossessWithAuth;
};
