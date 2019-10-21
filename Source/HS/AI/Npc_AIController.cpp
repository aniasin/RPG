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
#include "BehaviorTree/BlackboardComponent.h"
#include "Math/Vector.h"

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
		AICharacter->GetCharacterMovement()->RotationRate.Yaw = 250.0f;
		RunBehaviorTree(BehaviorTree);
		BlackboardComponent = GetBlackboardComponent();

		if (AICharacter->bIsCivilian)
		{
			BlackboardComponent->SetValueAsObject("IsCivilian", this);
		}
		else
		{
			BlackboardComponent->ClearValue("IsCivilian");
		}
		CheckHealth = AICharacter->GetHealth();
	}
}

FText ANpc_AIController::GetCharacterName()
{
	return AICharacter->CharacterName;
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

void ANpc_AIController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	if (!AICharacter) { return; }
	//Retrieving perceived actors
	TArray<AActor*> SeenActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(TSubclassOf<UAISense_Sight>(), SeenActors);

	// Numbers of seen actors and if they enter or exit view
	int32 NumberOfActorsSeen = SeenActors.Num();

	LastKnownPlayerPosition = Stimulus.StimulusLocation;
	bCanSeeActor = Stimulus.WasSuccessfullySensed();	

	if (AICharacter->bIsCivilian) { return; }

	ACharacterV2* Hero = Cast<ACharacterV2>(Actor);
	// Gain Sight
	if (bCanSeeActor && Hero)
	{
		// When gaining sight if already in alert, reset any timer running
		if (BlackboardComponent->GetValueAsBool("InAlert"))
		{ 
			UWorld* World = GetWorld();
			if (!World) { return; }
			World->GetTimerManager().ClearTimer(SearchTimerHandle);
			StartAlert();
		}
		else
		{
			StartAlert();
			AICharacter->SwitchCombat();
		}
		SeenPlayers.AddUnique(Hero);
		BlackboardComponent->SetValueAsObject("Player", Hero);
		UE_LOG(LogTemp, Warning, TEXT("%s: Gain Sight with %s!"), *AICharacter->CharacterName.ToString(), *Actor->GetName())
	}
	// sight is lost
	if (!bCanSeeActor && Hero)
	{
		// remember player's position and direction and a random one, set BB accordingly
		BlackboardComponent->SetValueAsVector("LastKnownPlayerPos", LastKnownPlayerPosition);
		LastKnownPlayerDirection = SeenPlayers[0]->GetVelocity().GetUnsafeNormal();
		BlackboardComponent->SetValueAsVector("NextSearchLocation", GetRandomSearchLocation(LastKnownPlayerDirection, AICharacter->SearchRadius));
		BlackboardComponent->SetValueAsVector("RandomSearchLocation", GetRandomSearchLocation(AICharacter->GetActorLocation(), AICharacter->SearchRadius));
		// Remove this player from SeenActors
		SeenPlayers.RemoveSingle(Hero);
		UE_LOG(LogTemp, Warning, TEXT("%s: Loose Sight with %s!"), *AICharacter->CharacterName.ToString(), *Actor->GetName())
		if (SeenPlayers.Num() <= 0) 
		{ 
			BlackboardComponent->ClearValue("Player"); 
		}
		else
		{
			BlackboardComponent->SetValueAsObject("Player", SeenPlayers[0]);
		}
		// if In alert Starting up a timer for how long npc will stay in alert, aka search
		if (BlackboardComponent->GetValueAsBool("InAlert"))
		{
			UWorld* World = GetWorld();
			if (!World || !AICharacter) { return; }
			SearchTimerDelegate.BindUFunction(this, FName("EndAlert"));
			World->GetTimerManager().SetTimer(SearchTimerHandle, SearchTimerDelegate, AICharacter->SearchTime, false);
		}
	}
}

void ANpc_AIController::StartAlert()
{
	if (!AICharacter || !BlackboardComponent) { return; }
	BlackboardComponent->SetValueAsBool("InAlert", true);
	AICharacter->GetCharacterMovement()->RotationRate.Yaw = 540.0f;

	UE_LOG(LogTemp, Warning, TEXT("%s: Alert..."), *AICharacter->CharacterName.ToString())
}

void ANpc_AIController::EndAlert()
{
	if (!AICharacter || !BlackboardComponent) { return; }
	UWorld* World = GetWorld();
	if (!World) { return; }
	World->GetTimerManager().ClearTimer(SearchTimerHandle);
	BlackboardComponent->SetValueAsBool("InAlert", false);
	AICharacter->GetCharacterMovement()->RotationRate.Yaw = 250.0f;

	UE_LOG(LogTemp, Warning, TEXT("%s: End Searching..."), *AICharacter->CharacterName.ToString())
	AICharacter->SwitchCombat();
}

//Combat
void ANpc_AIController::SetCombatBehavior()
{
	//TODO Logic to set combat behavior which could be: Flee, Retreat, Aggressive, Defensive, Neutral...
	if (!AICharacter) { return; }
	if (AICharacter->GetHealth() < CheckHealth)
	{
		BlackboardComponent->SetValueAsBool("Attack", false);
		UE_LOG(LogTemp, Error, TEXT("%s Is Now Setting Combat Behavior: Defend!"), *AICharacter->CharacterName.ToString())
	}
	else
	{
		BlackboardComponent->SetValueAsBool("Attack", true);
		UE_LOG(LogTemp, Error, TEXT("% Is Now Setting Combat Behavior: Attack!"), *AICharacter->CharacterName.ToString())

	}
	CheckHealth = AICharacter->GetHealth();
}

void ANpc_AIController::AttackTarget()
{
	if (!AICharacter) { return; }
	if (bAttacking)	{return;}
	bAttacking = true;
	UE_LOG(LogTemp, Warning, TEXT("%s is Performing Attack!"), *AICharacter->CharacterName.ToString())

 	AICharacter->AIPerformMeleeAttack();
	SetCombatBehavior();

 	AttackTimerDelegate.BindUFunction(this, FName("UpdateAttack"));
 	float CooldownTime = .75f;
	UWorld* World = GetWorld();
	if (!World) { return; }
	World->GetTimerManager().SetTimer(AttackTimerHandle, AttackTimerDelegate, CooldownTime, false);

}

void ANpc_AIController::UpdateAttack()
{
	if (!AICharacter) { return; }
	UE_LOG(LogTemp, Warning, TEXT("%s: End Attack!"), *AICharacter->CharacterName.ToString())
	bAttacking = false;

	UWorld* World = GetWorld();
	if (!World) { return; }
	World->GetTimerManager().ClearTimer(AttackTimerHandle);
}

void ANpc_AIController::Defend()
{
	if (!AICharacter) { return; }
	if (bDefending)	{return;}
	bDefending = true;
	UE_LOG(LogTemp, Warning, TEXT("%s is Performing ShieldUp!"), *AICharacter->CharacterName.ToString())

	AICharacter->AIPerformShieldUp();
	SetCombatBehavior();

	DefendTimerDelegate.BindUFunction(this, FName("UpdateDefend"));
	float CooldownTime = .75f;
	UWorld* World = GetWorld();
	if (!World) { return; }
	World->GetTimerManager().SetTimer(DefendTimerHandle, DefendTimerDelegate, CooldownTime, false);
}

void ANpc_AIController::UpdateDefend()
{
	if (!AICharacter) { return; }
	UE_LOG(LogTemp, Warning, TEXT("%s: End ShieldUp!"), *AICharacter->CharacterName.ToString())
	bDefending = false;

	UWorld* World = GetWorld();
	if (!World) { return; }
	World->GetTimerManager().ClearTimer(DefendTimerHandle);
}


FVector ANpc_AIController::GetRandomSearchLocation(FVector Origin, float Radius)
{
	UWorld* World = GEngine->GetWorld();
	if ensure(!World) { return Origin; }

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if ensure(!NavSystem) { return Origin; }
	FNavLocation Result;

	bool bFoundPath = NavSystem->GetRandomReachablePointInRadius(Origin, Radius, Result);
	return Result.Location;
}


void ANpc_AIController::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GEngine->GetWorld();
	if (!World)	{return;}
	World->GetTimerManager().ClearAllTimersForObject(this);
}
