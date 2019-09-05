// SillikOne.


#include "BTService_UpdateChasing.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Npc_AIController.h"
#include "NavigationSystem.h"
#include "HSCharacter.h"


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
			Player = Cast<AHSCharacter>(FoundActors[0]);
		}
	}
 }

void UBTService_UpdateChasing::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	BlackboardComponent->SetValueAsBool(CanSeePlayerKey.SelectedKeyName, ChasingController->bCanSeePlayer);

	if (ChasingController->bCanSeePlayer)
	{
		BlackboardComponent->SetValueAsVector(CurrentPlayerPositionKey.SelectedKeyName, PlayerLocation);
		BlackboardComponent->SetValueAsObject(PlayerKey.SelectedKeyName, Player);
	}

	// update last known position of the player
	if (!ChasingController->bCanSeePlayer)
	{
		BlackboardComponent->ClearValue(PlayerKey.SelectedKeyName);
		BlackboardComponent->SetValueAsVector(LastKnownPositionKey.SelectedKeyName, ChasingController->LastKnownPlayerPosition);
		BlackboardComponent->SetValueAsVector(LastKnownDirectionKey.SelectedKeyName, ChasingController->LastKnownPlayerDirection);
		BlackboardComponent->SetValueAsVector(NextSearchLocationKey.SelectedKeyName, (ChasingController->LastKnownPlayerPosition + ChasingController->LastKnownPlayerDirection * 1000));		
	}

	// update last can see Player
	bLastCanSeePlayer = ChasingController->bCanSeePlayer;

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}

