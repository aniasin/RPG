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

	UPROPERTY(EditDefaultsOnly, Category = AI)
		float SightRange = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = AI)
		float LoseSightRange = 2500.f;
	UPROPERTY(EditDefaultsOnly, Category = AI)
		float HearingRange = 1000.f;

	UFUNCTION()
		void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

public:
	void TakeObject(AActor* OtherActor);

protected:
	void OnPossess(APawn* InPawn);

};
