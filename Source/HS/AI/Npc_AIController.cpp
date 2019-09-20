// SillikOne.


#include "Npc_AIController.h"
#include "CharacterV2.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionTypes.h"
#include "Engine.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "BrainComponent.h"
#include "Abilities/HSAttributeSetBase.h"
#include "Abilities/HSAbilitySystemComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GenericTeamAgentInterface.h"

ANpc_AIController::ANpc_AIController()
{
	bWantsPlayerState = true;

	// Creating Perception component
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(FName("HearingConfig"));

	// Configure the sight and hearing sense
	SightConfig->SightRadius = SightRange;
	SightConfig->LoseSightRadius = LoseSightRange;
	HearingConfig->HearingRange = HearingRange;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	// Assign the sight and hearing sense to the perception component
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this,	&ANpc_AIController::OnTargetPerceptionUpdate);

	// Assign this controller to team 10
	SetGenericTeamId(FGenericTeamId(10));
}

void ANpc_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AICharacter = Cast<ACharacterV2>(InPawn);
	if (AICharacter && BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

ETeamAttitude::Type ANpc_AIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			return Super::GetTeamAttitudeTowards(*OtherPawn->GetController());
		}
	}
	return ETeamAttitude::Neutral;
}

void ANpc_AIController::SetCombatBehavior()
{
	//TODO Logic to set combat behavior which could be: Flee, Retreat, Aggressive, Defensive, Neutral...
	UE_LOG(LogTemp, Warning, TEXT("Now Setting Combat Behavior!"))

// 	
// 	AICharacter->SetCombatBehavior();
// 	AttackTarget();
// }
// 
// void ANpc_AIController::SwitchCombat()
// {
// 	AICharacter->SwitchCombat();
 }

void ANpc_AIController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	//Retrieving perceived actors
	TArray<AActor*> SeenActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(TSubclassOf<UAISense_Sight>(), SeenActors);

	// Numbers of seen actors and if they enter or exit view
	int32 NumberOfActorsSeen = SeenActors.Num();

	LastKnownPlayerPosition = Stimulus.StimulusLocation;
	bCanSeeActor = Stimulus.WasSuccessfullySensed();

	UWorld* World = GetWorld();
	if (!World) { return; }

	ACharacterV2* Hero = Cast<ACharacterV2>(Actor);
	// sight is lost
	if (!bCanSeeActor && Hero)
	{
		// remember player's direction and remove from array
		LastKnownPlayerDirection = SeenPlayers[0]->GetVelocity().GetUnsafeNormal();
		SeenPlayers.RemoveSingle(Hero);
		if (SeenPlayers.Num() == 0) { bAPlayerIsSeen = false; }
		UE_LOG(LogTemp, Warning, TEXT("%s: Loose Sight with %s!"), *AICharacter->CharacterName.ToString(), *Actor->GetName())

		AICharacter->Status = EStatus::InAlert;
		bIsInAlert = true;

		// Starting up a timer for how long npc will search
		SearchTimerDelegate.BindUFunction(this, FName("EndAlert"));
		World->GetTimerManager().SetTimer(SearchTimerHandle, SearchTimerDelegate, AICharacter->SearchTime, false);
	}
	// Gain Sight
	if (bCanSeeActor && Hero)
	{
		bAPlayerIsSeen = true;
		SeenPlayers.AddUnique(Hero);
		UE_LOG(LogTemp, Warning, TEXT("%s: Gain Sight with %s!"), *AICharacter->CharacterName.ToString(), *Actor->GetName())
		if (AICharacter->Status == EStatus::InCombat || AICharacter->Status == EStatus::InAlert) { return; }

		AICharacter->SwitchCombat();
		World->GetTimerManager().ClearTimer(SearchTimerHandle);
	}
}

void ANpc_AIController::EndAlert()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: End Searching..."), *AICharacter->CharacterName.ToString())
	AICharacter->SwitchCombat();
	AICharacter->Status = EStatus::InPeace;
	bIsInAlert = false;
}

FVector ANpc_AIController::GetRandomSearchLocation(float Radius)
{
	FVector Origin = AICharacter->GetActorLocation();
	FNavLocation Result;

	UWorld* World = GEngine->GetWorld();
	if ensure(!World){return Origin;}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if ensure (!NavSystem){	return Origin;	}

	bool bFoundPath = NavSystem->GetRandomReachablePointInRadius(Origin, Radius, Result);

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
	AICharacter->Status = EStatus::InCombat;
	UE_LOG(LogTemp, Warning, TEXT("%s is Performing Attack!"), *AICharacter->CharacterName.ToString())
// 	//Store New Request
// 	StoreAttackRequestID();
 	AICharacter->AIPerformMeleeAttack();
// 
 	AttackTimerDelegate.BindUFunction(this, FName("UpdateAttack"));
 	float CooldownTime = 1.0f;
	UWorld* World = GetWorld();
	if (!World) { return; }
	World->GetTimerManager().ClearTimer(SearchTimerHandle);
	World->GetTimerManager().SetTimer(AttackTimerHandle, AttackTimerDelegate, CooldownTime, false);

}

void ANpc_AIController::UpdateAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: End Attack!"), *AICharacter->CharacterName.ToString())
// 	FAIMessage Msg(UBrainComponent::AIMessage_MoveFinished, this, AttackRequestID, FAIMessage::Success);
// 	FAIMessage::Send(this, Msg);
	bAttacking = false;

	UWorld* World = GetWorld();
	if (!World) { return; }
	World->GetTimerManager().ClearTimer(AttackTimerHandle);
// /*	AttackRequestID = FAIRequestID::InvalidRequest;*/
}

void ANpc_AIController::Defend()
{
// 	if (bDefending)	{return;}
// 	bDefending = true;
// 	AICharacter->LeftHand();
// 
// 	AttackTimerDelegate.BindUFunction(this, FName("UpdateDefend"));
// 	float CooldownTime = AICharacter->AttributesComponent->AttackSpeed.GetCurrentValue();
// 	GetWorldTimerManager().SetTimer(AttackTimerHandle, AttackTimerDelegate, CooldownTime, false);
}

void ANpc_AIController::UpdateDefend()
{
// 	bDefending = false;
// 
// 	UWorld* World = GEngine->GetWorld();
// 	if (!World) { return; }
// 	World->GetTimerManager().ClearTimer(AttackTimerHandle);
}

void ANpc_AIController::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GEngine->GetWorld();
	if (!World)	{return;}
	World->GetTimerManager().ClearAllTimersForObject(this);
}
