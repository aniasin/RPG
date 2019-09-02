// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_AggroCheck.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UBTDecorator_AggroCheck : public UBTDecorator
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category = Blackboard)
		FBlackboardKeySelector VariableToCheck;

public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)const override;
};
