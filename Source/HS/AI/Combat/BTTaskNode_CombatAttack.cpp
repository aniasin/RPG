// SillikOne.


#include "BTTaskNode_CombatAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Npc_AIController.h"
#include "Classes/GameFramework/Character.h"
#include "HSCharacter.h"

EBTNodeResult::Type UBTTaskNode_CombatAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Check Blackboard component and controlled pawn
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	ACharacter* ControlledCharacter = OwnerComp.GetAIOwner()->GetCharacter();
	if (!BlackboardComponent || !ControlledCharacter)
	{
		return EBTNodeResult::Failed;
	}

	FBTCombatAttackTaskMemory* CombatAttack_Memory = (FBTCombatAttackTaskMemory*)NodeMemory;
	UE_LOG(LogTemp, Warning, TEXT("Attacking!"))

	ANpc_AIController* CurrentController = Cast<ANpc_AIController>(OwnerComp.GetAIOwner());
	if ensure(!CurrentController)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO CONTROLLER IN BTTaskNod_CombatAttack!"))
			return EBTNodeResult::Aborted;
	}
	CurrentController->AttackTarget();

	// Wait for message to finish task
	const FAIRequestID RequestID = CurrentController->GetAttackRequestID();
	CombatAttack_Memory->AttackRequestID = RequestID;
	WaitForMessage(OwnerComp, UBrainComponent::AIMessage_MoveFinished, RequestID);
		

	return EBTNodeResult::InProgress;
}
