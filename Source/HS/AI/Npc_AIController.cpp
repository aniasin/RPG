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
#include "Engine.h"
#include "NavigationSystem.h"
#include "BrainComponent.h"
#include "Abilities/PlayerAttributesSet.h"

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

	AICharacter = Cast<ANpcCharacter>(InPawn);
	if (AICharacter)
	{
		UBehaviorTree* BehaviorTree = AICharacter->BehaviorTree;
		if (BehaviorTree)
		{
			RunBehaviorTree(BehaviorTree);
		}
	}
}

void ANpc_AIController::SetCombatBehavior()
{
	//TODO Logic to set combat behavior which could be: Flee, Retreat, Aggressive, Defensive, Neutral...
	//Set GameplayTag Accordingly (Combat.Behavior.x)
	UE_LOG(LogTemp, Warning, TEXT("Now Setting Combat Behavior!"))

	
	AICharacter->SetCombatBehavior();
	AttackTarget();
}

void ANpc_AIController::SwitchCombat()
{
	AICharacter->SwitchCombat();
}

void ANpc_AIController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	//Retrieving perceived actors
	TArray<AActor*> SeenActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(TSubclassOf<UAISense_Sight>(), SeenActors);

	// Numbers of seen actors and if they enter or exit view
	bool bIsSeen = SeenActors.Contains(Actor);
	int32 NumberOfActorsSeen = SeenActors.Num();

	// Check if sensed actor is Player
	if (Cast<AHSCharacter>(Actor))
	{
		LastKnownPlayerPosition = Stimulus.StimulusLocation;
		bCanSeePlayer = Stimulus.WasSuccessfullySensed();
		// when sight is lost remember player's direction
		if (!bIsSeen)
		{
			LastKnownPlayerDirection = Actor->GetActorForwardVector();
			AICharacter->SetCanSeePlayer(false);
			UE_LOG(LogTemp, Warning, TEXT("Loose Sight!"))

			AICharacter->Status = EStatus::InAlert;
			AICharacter->GameplayTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Alert")));

			SearchTimerDelegate.BindUFunction(this, FName("EndAlert"));
			GetWorldTimerManager().SetTimer(SearchTimerHandle, SearchTimerDelegate, AICharacter->SearchTime, false);
		}
		if (bIsSeen)
		{
			UE_LOG(LogTemp, Warning, TEXT("Gain Sight!"))
			AICharacter->SetCanSeePlayer(true);
			if (AICharacter->Status == EStatus::InCombat || AICharacter->Status == EStatus::InAlert) {return;}

			AICharacter->SwitchCombat();

			UWorld* World = GEngine->GetWorld();
			if (!World)	{return;}
			World->GetTimerManager().ClearTimer(SearchTimerHandle);
		}
	}
}

void ANpc_AIController::EndAlert()
{
	if (AICharacter->Status == EStatus::InAlert)
	{
		AICharacter->SwitchCombat();
	}

	AICharacter->GameplayTags.RemoveTag(FGameplayTag::RequestGameplayTag(FName("Alert")));
}

FVector ANpc_AIController::GetRandomSearchLocation(float radius)
{
	FVector Origin = AICharacter->GetActorLocation();
	FNavLocation Result;

	UWorld* World = GEngine->GetWorld();
	if ensure(!World){return Origin;}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if ensure (!NavSystem){	return Origin;	}

	bool bFoundPath = NavSystem->GetRandomReachablePointInRadius(Origin, radius, Result);

	return Result.Location;
}

//Combat
void ANpc_AIController::AttackTarget()
{
	if (bAttacking)
	{
// 		FAIMessage Msg(UBrainComponent::AIMessage_MoveFinished, this, AttackRequestID, FAIMessage::Failure);
// 		FAIMessage::Send(this, Msg);
		return;
	}
	bAttacking = true;
	UE_LOG(LogTemp, Warning, TEXT("AIController is Performing Attack!"))
// 	//Store New Request
// 	StoreAttackRequestID();
	AICharacter->RightHand();

 	AttackTimerDelegate.BindUFunction(this, FName("UpdateAttack"));
 	float CooldownTime = AICharacter->AttributesComponent->AttackSpeed.GetCurrentValue();
 	GetWorldTimerManager().SetTimer(AttackTimerHandle, AttackTimerDelegate, CooldownTime, false);

}

void ANpc_AIController::UpdateAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("AIController: End Attack!"))
// 	FAIMessage Msg(UBrainComponent::AIMessage_MoveFinished, this, AttackRequestID, FAIMessage::Success);
// 	FAIMessage::Send(this, Msg);
	bAttacking = false;

	UWorld* World = GEngine->GetWorld();
	if (!World) { return; }
	World->GetTimerManager().ClearTimer(AttackTimerHandle);
/*	AttackRequestID = FAIRequestID::InvalidRequest;*/
}

void ANpc_AIController::Defend()
{
	if (bDefending)	{return;}
	bDefending = true;
	AICharacter->LeftHand();

	AttackTimerDelegate.BindUFunction(this, FName("UpdateDefend"));
	float CooldownTime = AICharacter->AttributesComponent->AttackSpeed.GetCurrentValue();
	GetWorldTimerManager().SetTimer(AttackTimerHandle, AttackTimerDelegate, CooldownTime, false);
}

void ANpc_AIController::UpdateDefend()
{
	bDefending = false;

	UWorld* World = GEngine->GetWorld();
	if (!World) { return; }
	World->GetTimerManager().ClearTimer(AttackTimerHandle);
}

void ANpc_AIController::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GEngine->GetWorld();
	if (!World)	{return;}
	World->GetTimerManager().ClearAllTimersForObject(this);
}
