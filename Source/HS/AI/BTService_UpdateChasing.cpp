// SillikOne.


#include "BTService_UpdateChasing.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Npc_AIController.h"
#include "CharacterV2.h"


UBTService_UpdateChasing::UBTService_UpdateChasing(const FObjectInitializer& ObjectInitializer)
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_UpdateChasing::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get Blackboard
	 BlackboardComponent = OwnerComp.GetBlackboardComponent();
	 AIController = OwnerComp.GetAIOwner();
	if (!BlackboardComponent || !AIController) { return; }
	ChasingController = Cast<ANpc_AIController>(AIController);
 }

void UBTService_UpdateChasing::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	// Sight changed
	if (ChasingController->bAPlayerIsSeen != bLastCanSeePlayer)
	{
		// A player is Seen
		if (ChasingController->bAPlayerIsSeen)
		{
			PlayerToChase = ChasingController->SeenPlayers[0];
			BlackboardComponent->SetValueAsVector(CurrentPlayerPositionKey.SelectedKeyName, PlayerToChase->GetActorLocation());
			BlackboardComponent->SetValueAsObject(PlayerKey.SelectedKeyName, PlayerToChase);
		}
		// No Sees Player, set searching points
		if (!ChasingController->bAPlayerIsSeen && ChasingController->bIsInAlert)
		{
			BlackboardComponent->ClearValue(PlayerKey.SelectedKeyName);
			// Last position where player was seen
			BlackboardComponent->SetValueAsVector(LastKnownPositionKey.SelectedKeyName, ChasingController->LastKnownPlayerPosition);
			// From Last position seen, go to last direction seen
			BlackboardComponent->SetValueAsVector(NextSearchLocationKey.SelectedKeyName, (ChasingController->LastKnownPlayerPosition + ChasingController->LastKnownPlayerDirection * 1500));		
			// take another random location in range
			BlackboardComponent->SetValueAsVector(RandomSearchLocationKey.SelectedKeyName, ChasingController->GetRandomSearchLocation(SearchRadius));	

			UE_LOG(LogTemp, Warning, TEXT("Searching..."))
		}
	}

	// update last can see Player
	bLastCanSeePlayer = ChasingController->bAPlayerIsSeen;

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}

