// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_SearchAt.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UBTTask_SearchAt : public UBTTaskNode
{
	GENERATED_BODY()
	
		UPROPERTY(EditAnywhere, Category = BlackBoard)
		FBlackboardKeySelector CanSeePlayerKey;
	UPROPERTY(EditAnywhere, Category = BlackBoard)
		FBlackboardKeySelector NextMoveKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
