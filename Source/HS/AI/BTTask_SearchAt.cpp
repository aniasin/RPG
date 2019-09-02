// SillikOne.


#include "BTTask_SearchAt.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Npc_AIController.h"
#include "Classes/GameFramework/Character.h"
#include "HSCharacter.h"

EBTNodeResult::Type UBTTask_SearchAt::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Check Blackboard component and controlled pawn
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	ACharacter* ControlledCharacter = OwnerComp.GetAIOwner()->GetCharacter();
	if (!BlackboardComponent || !ControlledCharacter)
	{
		return EBTNodeResult::Failed;
	}
	// Abort if sees Player
	if (BlackboardComponent->GetValueAsBool(CanSeePlayerKey.SelectedKeyName))
	{
		return EBTNodeResult::Aborted;
	}
	AAIController* Controller = OwnerComp.GetAIOwner();
	ANpc_AIController* NpcController = Cast<ANpc_AIController>(Controller);

	FVector LocationToMove = BlackboardComponent->GetValueAsVector(NextMoveKey.SelectedKeyName);	
	NpcController->MoveToLocation(LocationToMove);

	return EBTNodeResult::Succeeded;
}
