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

	if (!PlayerKey.IsSet())
		{

		// Retrieve Player
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, FoundActors);
		if (FoundActors[0])
		{
			PlayerLocation = FoundActors[0]->GetActorLocation();
			Player = Cast<ACharacterV2>(FoundActors[0]);
		}
	}
 }

void UBTService_UpdateChasing::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (ChasingController->bCanSeePlayer)
	{
		BlackboardComponent->SetValueAsVector(CurrentPlayerPositionKey.SelectedKeyName, PlayerLocation);
		BlackboardComponent->SetValueAsObject(PlayerKey.SelectedKeyName, Player);
	}
	else if (!ChasingController->bCanSeePlayer)
	{
		BlackboardComponent->ClearValue(PlayerKey.SelectedKeyName);
	}

	// Sight changed
	if (ChasingController->bCanSeePlayer != bLastCanSeePlayer)
	{
		// No Sees Player, set searching points
		if (!ChasingController->bCanSeePlayer)
		{
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
	bLastCanSeePlayer = ChasingController->bCanSeePlayer;

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}

