// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_TakeObject.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UBTTaskNode_TakeObject : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = BlackBoard)
	FBlackboardKeySelector ObjectToTake;
	UPROPERTY(EditAnywhere, Category = BlackBoard)
		FBlackboardKeySelector CurrentWeapon;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
