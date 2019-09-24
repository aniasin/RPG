// SillikOne.


#include "BTDecorator_AggroCheck.h"
#include "AI/Npc_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UBTDecorator_AggroCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!BlackboardComponent || !AIController) { return false; }

	ANpc_AIController* ChasingController = Cast<ANpc_AIController>(AIController);
	return BlackboardComponent->GetValueAsBool(BoolToCheck.SelectedKeyName);
}
