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

	class ACharacterV2* AICharacter;

	class UAIPerceptionComponent* PerceptionComponent;
	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditDefaultsOnly, Category = NPC_AI)
		float SightRange = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = NPC_AI)
		float LoseSightRange = 2500.f;
	UPROPERTY(EditDefaultsOnly, Category = NPC_AI)
		float HearingRange = 1000.f;
	UPROPERTY(EditAnywhere, Category = NPC_AI)
		class UBehaviorTree* BehaviorTree;
	UBlackboardComponent* BlackboardComponent;


	UFUNCTION()
		void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

	// Combat
	bool bAttacking = false;
	bool bDefending = false;

	float CheckHealth;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason)override;
	
public:
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other)const override;

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
	FTimerHandle DefendTimerHandle;
	FTimerDelegate DefendTimerDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPC_AI)
	bool bCanSeeActor = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPC_AI)
	FVector LastKnownPlayerPosition;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = NPC_AI)
	FVector LastKnownPlayerDirection;

	FText GetCharacterName();

	UFUNCTION()
		void EndAlert();
	UFUNCTION()
		void StartAlert();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NPC_AI)
	TArray<ACharacterV2*>SeenPlayers;

	void SetCombatBehavior();

	FVector GetRandomSearchLocation(FVector Origin, float Radius);

protected:
	void OnPossess(APawn* InPawn);

};
