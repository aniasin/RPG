// SillikOne.


#include "Npc_AIController.h"
#include "NPC/NpcCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionTypes.h"
#include "HSCharacter.h"



ANpc_AIController::ANpc_AIController()
{
	// Creating Perception component
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(FName("HearingConfig"));

	// Configure the sight and hearing sense
	SightConfig->SightRadius = SightRange;
	SightConfig->LoseSightRadius = LoseSightRange;
	HearingConfig->HearingRange = HearingRange;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// Assign the sight and hearing sense to the perception component
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this,	&ANpc_AIController::OnTargetPerceptionUpdate);

	// Assign this controller to team 1
	SetGenericTeamId(FGenericTeamId(1));
}

void ANpc_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ANpcCharacter* AICharacter = Cast<ANpcCharacter>(InPawn);
	if (AICharacter)
	{
		UBehaviorTree* BehaviorTree = AICharacter->BehaviorTree;
		if (BehaviorTree)
		{
			RunBehaviorTree(BehaviorTree);
		}
	}
}

void ANpc_AIController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	if (Cast<AHSCharacter>(Actor))
	{
		UE_LOG(LogTemp, Warning, TEXT("I CAN SEE YOU!"))
	}
}

