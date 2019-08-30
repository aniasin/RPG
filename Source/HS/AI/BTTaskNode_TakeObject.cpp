// SillikOne.


#include "BTTaskNode_TakeObject.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Npc_AIController.h"

EBTNodeResult::Type UBTTaskNode_TakeObject::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("TASK TAKEOBJECT IS RUNNING!"))
	// Check Blackboard component and controlled pawn
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!BlackboardComponent || !ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}
	//Retrieve object
	auto OtherObject = BlackboardComponent->GetValueAsObject(TEXT("WeaponQueryed"));
	AActor* OtherActor = Cast<AActor>(OtherObject);
	// Call AIController to take object
	ANpc_AIController* AIController = Cast<ANpc_AIController>(OwnerComp.GetAIOwner());
	AIController->TakeObject(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("Trying to grab! %s"), *OtherActor->GetName())
	BlackboardComponent->ClearValue(ObjectToTake.SelectedKeyName);
	return EBTNodeResult::Succeeded;
}
