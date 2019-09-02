// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_CombatDefend.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UBTTaskNode_CombatDefend : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, Category = Blackboard)
		FBlackboardKeySelector TargetKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
