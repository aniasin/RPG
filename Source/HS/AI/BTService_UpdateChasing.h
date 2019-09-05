// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateChasing.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UBTService_UpdateChasing : public UBTService
{
	GENERATED_BODY()
		UBTService_UpdateChasing(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UPROPERTY(EditAnywhere, Category = Blackboard)
		FBlackboardKeySelector CurrentPlayerPositionKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
		FBlackboardKeySelector LastKnownPositionKey;

	UPROPERTY(EditAnywhere, Category = PlayerClass)
		TSubclassOf<AActor> PlayerClass;

	UPROPERTY(EditAnywhere, Category = Blackboard)
		FBlackboardKeySelector PlayerKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
		FBlackboardKeySelector NextSearchLocationKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
		FBlackboardKeySelector RandomSearchLocationKey;

	UPROPERTY(EditAnywhere, Category = Parameter)
		float Radius;

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;

private:
	class ANpc_AIController* ChasingController;
	class AAIController* AIController;
	class UBlackboardComponent* BlackboardComponent;
	bool bLastCanSeePlayer = false;
	FVector PlayerLocation;
	AActor* Player;
};
