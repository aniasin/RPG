// SillikOne.


#include "BTDecorator_AggroCheck.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UBTDecorator_AggroCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) { return false; }

	// -1 = Flee, 0 = Neutral, 1 = Aggressive, 2 = Defensive
	int32 CurrentAggro = BlackboardComponent->GetValueAsInt(VariableToCheck.SelectedKeyName);
	if (CurrentAggro >= 1)
	{
		return true;
	}

	return false;
}
