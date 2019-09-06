// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Npc_AIController.generated.h"

/**
 * 
 */
UCLASS()
class HS_API ANpc_AIController : public AAIController
{
	GENERATED_BODY()
		ANpc_AIController();

	class ANpcCharacter* AICharacter;

	class UAIPerceptionComponent* PerceptionComponent;
	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditDefaultsOnly, Category = NPC_AI)
		float SightRange = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = NPC_AI)
		float LoseSightRange = 2500.f;
	UPROPERTY(EditDefaultsOnly, Category = NPC_AI)
		float HearingRange = 1000.f;


	UFUNCTION()
		void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

	// Combat
	bool bAttacking = false;
	bool bDefending = false;

	//AI TaskMemory
	uint32 NextRequestID;
	FAIRequestID AttackRequestID;

	FORCEINLINE void StoreAttackRequestID() { AttackRequestID = NextRequestID++; }

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason)override;
	
public:
	FORCEINLINE FAIRequestID GetAttackRequestID() const { return AttackRequestID; }

	void AttackTarget();
	void Defend();

	UFUNCTION()
	void UpdateAttack();
	UFUNCTION()
		void UpdateDefend();

	/* Handle to manage timer */
	FTimerHandle AttackTimerHandle;
	FTimerDelegate AttackTimerDelegate;
	FTimerHandle SearchTimerHandle;
	FTimerDelegate SearchTimerDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPC_AI)
	bool bCanSeePlayer;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPC_AI)
	FVector LastKnownPlayerPosition;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPC_AI)
	FVector LastKnownPlayerDirection;
	// Set how long npc will search for player after loosing sight


	UFUNCTION()
		void EndAlert();

	void SetCombatBehavior();
	void SwitchCombat();

	FVector GetRandomSearchLocation(float Radius);

protected:
	void OnPossess(APawn* InPawn);

};
