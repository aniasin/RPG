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

FDialogues_Struct UDialogueComponent::MakeDialogueStruct(int32 Priority, float Time, float DurationInMemory, 
	FText Sentence, FVector Site, FString SiteName, bool bPointAt)
{
	DefaultDialogue.Priority = Priority;
	DefaultDialogue.Time = Time;
	DefaultDialogue.DurationInMemory = DurationInMemory;
	DefaultDialogue.Sentence = Sentence;
	DefaultDialogue.Site = Site;
	DefaultDialogue.SiteName = SiteName;
	DefaultDialogue.bPointAt = bPointAt;

	return DefaultDialogue;
}

// Called when the game starts
void UDialogueComponent::BeginPlay()
{
	OwnerActor = Cast<ACharacterV2>(GetOwner());

	Super::BeginPlay();

	// Default NPC dialog wich will be at index 0
	FVector HomeLocation = OwnerActor->GetActorLocation();
	DefaultDialogue = MakeDialogueStruct(-1, 0, 0, FText::FromString(DefaultDialogueSentence), HomeLocation, FString("My place."), false);
	DialogArray.Add(DefaultDialogue);
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
				OwnerActor->BeginDialogue(OverlapingCharacter);
			}
			else
			{
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
	// 	for (int32 i = 0; i <= DialogArray.Num(); i++)
	// 	{
	// 		return DialogArray[i].Sentence;
	// 	}
	return DialogArray[0];
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
