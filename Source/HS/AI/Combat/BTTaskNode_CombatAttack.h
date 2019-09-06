// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_CombatAttack.generated.h"


// struct FBTCombatAttackTaskMemory
// {
// 	/** Move request ID */
// 	FAIRequestID AttackRequestID;
// 
// 	FDelegateHandle BBObserverDelegateHandle;
// 	FVector PreviousGoalLocation;
// 
// 	TWeakObjectPtr<class UAITask_MoveTo> Task;
// 
// 	uint8 bObserverCanFinishTask : 1;
// };
/**
 * 
 */
UCLASS()
class HS_API UBTTaskNode_CombatAttack : public UBTTaskNode
{
	GENERATED_BODY()

	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
