// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateCombatBehavior.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UBTService_UpdateCombatBehavior : public UBTService
{
	GENERATED_BODY()
	
		UBTService_UpdateCombatBehavior(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
};
