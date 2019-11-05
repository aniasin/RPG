// SillikOne.


#include "DialogueComponent.h"
#include "UnrealNetwork.h"
#include "Engine/EngineTypes.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "CharacterV2.h"

// Sets default values for this component's properties
UDialogueComponent::UDialogueComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bReplicates = true;
}

void UDialogueComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDialogueComponent, DialogArray);
}

FDialogues_Struct UDialogueComponent::MakeDialogueStruct(int32 Priority, float Time, float DurationInMemory, int32 WitnessLevel,
	FText Sentence, FVector Site, FString SiteName, bool bPointAt, TArray<ACharacterV2*> MarkedCharacters)
{
	FDialogues_Struct DefaultDialogue;
	DefaultDialogue.Priority = Priority;
	DefaultDialogue.Time = Time;
	DefaultDialogue.DurationInMemory = DurationInMemory;
	DefaultDialogue.WitnessLevel = WitnessLevel;
	DefaultDialogue.Sentence = Sentence;
	DefaultDialogue.Site = Site;
	DefaultDialogue.SiteName = SiteName;
	DefaultDialogue.bPointAt = bPointAt;
	DefaultDialogue.MarkedCharacters = MarkedCharacters;

	return DefaultDialogue;
}

void UDialogueComponent::MarkCharacter(int32 IndexToMark)
{
	// add currently focused actor to the array of people who already heard that from me
	ACharacterV2* CharacterToAdd = Cast<ACharacterV2>(OwnerActor->GetCurrentFocusedActor());
	DialogArray[IndexToMark].MarkedCharacters.AddUnique(CharacterToAdd);
}

// Called when the game starts
void UDialogueComponent::BeginPlay()
{
	OwnerActor = Cast<ACharacterV2>(GetOwner());

	Super::BeginPlay();

	FVector HomeLocation = OwnerActor->GetActorLocation();
	TArray<ACharacterV2*>Mark;
	// Default NPC dialog which will be at index 0
	FDialogues_Struct ONE = MakeDialogueStruct(0, 1, 0,  0, FText::FromString("Everything is quiet \naround here."), 
		HomeLocation, FString("My place."), false, Mark);
	DialogArray.Add(ONE);
}

// Called every frame
void UDialogueComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDialogueComponent::OnOverlapDialogueBegin_Implementation(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if (OwnerActor->HasAuthority())
	{
		if (OwnerActor->GetCurrentFocusedActor() != nullptr) { return; } // NPC has already something focused

		if (OwnerActor->bIsCivilian) // Only civilians speak for now...
		{
			ACharacterV2* OverlapingCharacter = Cast<ACharacterV2>(OtherActor);
			if (OverlapingCharacter == GetOwner() || !OverlapingCharacter || !OwnerActor) { return; }
			OwnerActor->SetCurrentFocusedActor(OtherActor);

			if (!OverlapingCharacter->InteractionWidget) // Overlaping character is not Player
			{
				bCurrentSpeakerIsPlayer = false;
				OwnerActor->ToggleMovement(false);
				OwnerActor->BeginDialogue(OverlapingCharacter);
			}
			else
			{

				bCurrentSpeakerIsPlayer = true;
				OverlapingCharacter->ToggleInteractionWidget(OwnerActor); // Press Interaction to speak

			}

			UWorld* World = GetWorld();
			if (!World || !OverlapingCharacter) { return; }
			ConversationTimerDelegate.BindUFunction(this, FName("EndNPCDialogue"));
			World->GetTimerManager().SetTimer(ConversationTimerHandle, ConversationTimerDelegate, 10, false);
		}
	}
}

void UDialogueComponent::EndNPCDialogue()
{
	OwnerActor->ToggleMovement(true);
	
	UWorld* World = GetWorld();
	if (!World) { return; }
	World->GetTimerManager().ClearTimer(ConversationTimerHandle);
}

FDialogues_Struct UDialogueComponent::ChooseDialogue()
{
	if (DialogArray.Num() <= 1) { return DialogArray[0]; } // If no more option than default

	// We got some alternative
	// Make an array to store the possible sentences
	TArray<FDialogues_Struct>PossibleSentences;	

	for (int32 i = 0; i < DialogArray.Num(); i++)
	{
		// Grab only the ones we never told to the current speaker
		ACharacterV2* CharacterToCompare = Cast<ACharacterV2>(OwnerActor->GetCurrentFocusedActor());
		if (!DialogArray[i].MarkedCharacters.Contains(CharacterToCompare))
		{
			PossibleSentences.AddUnique(DialogArray[i]);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DIALOG: Found something I already told you!"))
		}
	}

	// Sort by Priority 0 = top priority
	PossibleSentences.Sort();
	// Mark this sentence as already told to this speaker
	if (int32 IndexToMark = DialogArray.Find(PossibleSentences[0]))
	{
		MarkCharacter(IndexToMark);
		// if the Info to tell is priority -1 (important) pass it to the other NPC
		if (DialogArray[IndexToMark].Priority < 0 && !bCurrentSpeakerIsPlayer)
		{
			PassInfoToSpeaker(DialogArray[IndexToMark], OwnerActor->GetCurrentFocusedActor());
		}
		// return the first and best choice
		return DialogArray[IndexToMark];
	}
	
	// still there? return the default
	return DialogArray[0];
}



void UDialogueComponent::PassInfoToSpeaker(FDialogues_Struct InfoToPass, AActor* OtherSpeaker)
{
	ACharacterV2* OtherCharacter = Cast<ACharacterV2>(OtherSpeaker);
	if (!OtherCharacter) { return; }

	TArray<ACharacterV2*> Characters;
	// Info to pass has to be less precise than from original witness
	FDialogues_Struct TranformedInfoToPass = MakeDialogueStruct(
		InfoToPass.Priority,
		InfoToPass.Time,
		InfoToPass.DurationInMemory,
		1,								// Witness level
		InfoToPass.Sentence,
		InfoToPass.Site,
		InfoToPass.SiteName,
		InfoToPass.bPointAt,
		Characters);

/*	OtherCharacter->DialogueComponent->DialogArray.AddUnique(TranformedInfoToPass);*/
	OwnerActor->SpawnGossipZone(TranformedInfoToPass);
}

void UDialogueComponent::OnOverlapDialogueEnd_Implementation(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OwnerActor->HasAuthority())
	{
		if (OwnerActor->GetCurrentFocusedActor() != OtherActor) { return; }
		ACharacterV2* OverlapingCharacter = Cast<ACharacterV2>(OtherActor);
		if (OverlapingCharacter == GetOwner() || !OverlapingCharacter || !OwnerActor) { return; }
		UE_LOG(LogTemp, Warning, TEXT("DIALOGUE: Overlap End! : %s"), *OtherActor->GetName())

		OverlapingCharacter->ToggleInteractionWidget(OwnerActor);
		OwnerActor->EndDialogue();
		OwnerActor->SetCurrentFocusedActor(nullptr);
		OwnerActor->ToggleMovement(true);
	}
}
