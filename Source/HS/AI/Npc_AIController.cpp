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

	AICharacter = Cast<ACharacterV2>(InPawn);
	if (AICharacter && BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
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

	ACharacterV2* Player = Cast<ACharacterV2>(Actor);
	// sight is lost
	if (!bCanSeeActor && Player)
	{
		// remember player's direction and remove from array
		LastKnownPlayerDirection = SeenPlayers[0]->GetVelocity().GetUnsafeNormal();
		SeenPlayers.RemoveSingle(Player);
		if (SeenPlayers.Num() == 0) { bAPlayerIsSeen = false; }
		UE_LOG(LogTemp, Warning, TEXT("Loose Sight! %s"), *Actor->GetName())

		AICharacter->Status = EStatus::InAlert;
		bIsInAlert = true;

		// Starting up a timer for how long npc will search
		SearchTimerDelegate.BindUFunction(this, FName("EndAlert"));
		World->GetTimerManager().SetTimer(SearchTimerHandle, SearchTimerDelegate, AICharacter->SearchTime, false);
	}
	// Gain Sight
	if (bCanSeeActor && Player)
	{
		bAPlayerIsSeen = true;
		SeenPlayers.AddUnique(Player);
		UE_LOG(LogTemp, Warning, TEXT("Gain Sight! %s"), *Actor->GetName())
		if (AICharacter->Status == EStatus::InCombat || AICharacter->Status == EStatus::InAlert) { return; }

		AICharacter->SwitchCombat();
		World->GetTimerManager().ClearTimer(SearchTimerHandle);
	}
}

void ANpc_AIController::EndAlert()
{
	UE_LOG(LogTemp, Warning, TEXT("End Searching..."))
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
	UE_LOG(LogTemp, Warning, TEXT("AIController is Performing Attack!"))
// 	//Store New Request
// 	StoreAttackRequestID();
// 	AICharacter->RightHand();
// 
//  	AttackTimerDelegate.BindUFunction(this, FName("UpdateAttack"));
//  	float CooldownTime = AICharacter->AttributesComponent->AttackSpeed.GetCurrentValue();
//  	GetWorldTimerManager().SetTimer(AttackTimerHandle, AttackTimerDelegate, CooldownTime, false);

}

void ANpc_AIController::UpdateAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("AIController: End Attack!"))
// 	FAIMessage Msg(UBrainComponent::AIMessage_MoveFinished, this, AttackRequestID, FAIMessage::Success);
// 	FAIMessage::Send(this, Msg);
// 	bAttacking = false;
// 
// 	UWorld* World = GEngine->GetWorld();
// 	if (!World) { return; }
// 	World->GetTimerManager().ClearTimer(AttackTimerHandle);
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
