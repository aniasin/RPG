// SillikOne.


#include "BTService_UpdateCombatBehavior.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Npc_AIController.h"

UBTService_UpdateCombatBehavior::UBTService_UpdateCombatBehavior(const FObjectInitializer& ObjectInitializer)
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_UpdateCombatBehavior::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get Blackboard
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) { return; }


}

void UBTService_UpdateCombatBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Check BB and AIController
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!BlackboardComp || !AIController) { return; }

	ANpc_AIController* FightingController = Cast<ANpc_AIController>(AIController);
	if (!FightingController) { return; }

	FightingController->SetCombatBehavior();
}

