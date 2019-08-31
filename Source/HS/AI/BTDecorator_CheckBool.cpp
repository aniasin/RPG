// SillikOne.


#include "BTDecorator_CheckBool.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UBTDecorator_CheckBool::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent){return false;}


	return BlackboardComponent->GetValueAsBool(BoolVariableToCheck.SelectedKeyName);
}
