// SillikOne.


#include "BTTaskNode_TakeObject.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Npc_AIController.h"
#include "Classes/GameFramework/Character.h"
#include "CharacterV2.h"

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
	auto OtherObject = BlackboardComponent->GetValueAsObject(ObjectToTake.SelectedKeyName);
	AActor* OtherActor = Cast<AActor>(OtherObject);

	// Call Controlled Character to take object
	ACharacterV2* NPC = Cast<ACharacterV2>(ControlledCharacter);
 	NPC->TakeItem(OtherActor);

	//clear EnvQuery targetObject so it can run again
	BlackboardComponent->ClearValue(ObjectToTake.SelectedKeyName);
	BlackboardComponent->SetValueAsObject(CurrentWeapon.SelectedKeyName, OtherActor);

	return EBTNodeResult::Succeeded;
}
