// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "HSCharacter.h"
#include "Perception/AIPerceptionTypes.h"
#include "NpcCharacter.generated.h"


UENUM(BlueprintType)
enum class ENpc_Type : uint8
{
	Aggressive UMETA(DisplayName = "Aggressive"),
	Peacefull UMETA(DisplayName = "Peacefull")
};
/**
 * 
 */
UCLASS()
class HS_API ANpcCharacter : public AHSCharacter
{
	GENERATED_BODY()

	ANpcCharacter();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		ENpc_Type Npc_Type;
	UPROPERTY(EditAnywhere, Category = AI)
		float SearchTime = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	float PercentHP;

	// Check if npc is fully equipped
	bool IsEquipped();
	void SetCombatBehavior();
	void SetCanSeePlayer(bool bCanSee);

protected:
	virtual void BeginPlay()override;

};
