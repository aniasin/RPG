// SillikOne.


#include "DialogueComponent.h"
#include "UnrealNetwork.h"
#include "Engine/EngineTypes.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "CharacterV2.h"

// Sets default values for this component's properties
UDialogueComponent::UDialogueComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bReplicates = true;

	DialogueTrigger = CreateDefaultSubobject<USphereComponent>(FName("DialogueTrigger"));
	DialogueTrigger->SetSphereRadius(100, true);
}

// Called when the game starts
void UDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = Cast<ACharacterV2>(GetOwner());
	if (!OwnerActor->bIsCivilian) { return; } // For now, only civilians can speak

	DialogueTrigger->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	DialogueTrigger->OnComponentBeginOverlap.AddDynamic(this, &UDialogueComponent::OnOverlapDialogueBegin);
	DialogueTrigger->OnComponentEndOverlap.AddDynamic(this, &UDialogueComponent::OnOverlapDialogueEnd);
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
		if (OwnerActor->GetCurrentFocusedActor() != nullptr) { return; }

		ACharacterV2* OverlapingCharacter = Cast<ACharacterV2>(OtherActor);
		if (OverlapingCharacter == GetOwner() || !OverlapingCharacter || !OwnerActor) { return; }
		OwnerActor->SetCurrentFocusedActor(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("DIALOGUE: Overlap Begin! : %s"), *OtherActor->GetName())
		
		OverlapingCharacter->ToggleInteractionWidget(OwnerActor);
	}
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
