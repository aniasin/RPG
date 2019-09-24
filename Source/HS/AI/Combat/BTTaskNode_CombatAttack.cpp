// SillikOne.


#include "BTTaskNode_CombatAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Npc_AIController.h"
#include "Classes/GameFramework/Character.h"
#include "CharacterV2.h"


EBTNodeResult::Type UBTTaskNode_CombatAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Check Blackboard component and controlled pawn
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	ACharacter* ControlledCharacter = OwnerComp.GetAIOwner()->GetCharacter();
	if (!BlackboardComponent || !ControlledCharacter)
	{
		return EBTNodeResult::Failed;
	}
	
	ANpc_AIController* CurrentController = Cast<ANpc_AIController>(OwnerComp.GetAIOwner());
	if (!CurrentController) {return EBTNodeResult::Aborted;	}

	CurrentController->AttackTarget();

	return EBTNodeResult::Succeeded;
}
