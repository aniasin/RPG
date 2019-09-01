// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateChasing.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UBTService_UpdateChasing : public UBTService
{
	GENERATED_BODY()
		UBTService_UpdateChasing(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector CanSeePlayerKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
		FBlackboardKeySelector CurrentPlayerPositionKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
		FBlackboardKeySelector LastKnownPositionKey;

	UPROPERTY(EditAnywhere, Category = PlayerClass)
		TSubclassOf<AActor> PlayerClass;

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;

private:

	bool bLastCanSeePlayer;
};
