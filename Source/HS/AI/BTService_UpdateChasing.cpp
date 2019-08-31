// SillikOne.


#include "BTService_UpdateChasing.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Npc_AIController.h"


UBTService_UpdateChasing::UBTService_UpdateChasing(const FObjectInitializer& ObjectInitializer)
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_UpdateChasing::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get Blackboard
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent){return;}

	if (!PlayerKey.IsSet())
	{
		// Retrieve Player and updateBlackboard
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, FoundActors);
		if (FoundActors[0])
		{
			BlackboardComponent->SetValueAsObject(PlayerKey.SelectedKeyName, FoundActors[0]);
		}
	}
}

void UBTService_UpdateChasing::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Check BB and AIController
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!BlackboardComp || !AIController){	return;	}

	ANpc_AIController* ChasingController = Cast<ANpc_AIController>(AIController);
	if (!ChasingController)	{return;}

	BlackboardComp->SetValueAsBool(CanSeePlayerKey.SelectedKeyName, ChasingController->bCanSeePlayer);
	// update last known position of the player
	if (ChasingController->bCanSeePlayer != bLastCanSeePlayer)
	{
		BlackboardComp->SetValueAsVector(LastKnownPositionKey.SelectedKeyName, ChasingController->LastKnownPlayerPosition);
	}

	// update last can see Player
	bLastCanSeePlayer = ChasingController->bCanSeePlayer;

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
