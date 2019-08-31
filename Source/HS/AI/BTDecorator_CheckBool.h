// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckBool.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UBTDecorator_CheckBool : public UBTDecorator
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, Category = Blackboard)
		FBlackboardKeySelector BoolVariableToCheck;

public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)const override;
};
