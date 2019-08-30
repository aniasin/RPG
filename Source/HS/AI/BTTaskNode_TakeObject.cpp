// SillikOne.


#include "BTTaskNode_TakeObject.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Npc_AIController.h"
#include "Classes/GameFramework/Character.h"
#include "HSCharacter.h"

EBTNodeResult::Type UBTTaskNode_TakeObject::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Check Blackboard component and controlled pawn
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	ACharacter* ControlledCharacter = OwnerComp.GetAIOwner()->GetCharacter();
	if (!BlackboardComponent || !ControlledCharacter)
	{
		return EBTNodeResult::Failed;
	}

	//Retrieve object
	auto OtherObject = BlackboardComponent->GetValueAsObject(TEXT("WeaponQueryed"));
	AActor* OtherActor = Cast<AActor>(OtherObject);

	// Call Controlled Character to take object
	AHSCharacter* NPC = Cast<AHSCharacter>(ControlledCharacter);
 	NPC->TakeObject(OtherActor);

	//clear EnvQuery targetObject so it can run again
	BlackboardComponent->ClearValue(ObjectToTake.SelectedKeyName);

	return EBTNodeResult::Succeeded;
}
