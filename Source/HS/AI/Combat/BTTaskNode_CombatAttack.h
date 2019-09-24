// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_CombatAttack.generated.h"


UCLASS()
class HS_API UBTTaskNode_CombatAttack : public UBTTaskNode
{
	GENERATED_BODY()

	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
