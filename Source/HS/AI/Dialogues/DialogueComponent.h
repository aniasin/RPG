// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Internationalization/Text.h"
#include "GameFramework/OnlineReplStructs.h"
#include "DialogueComponent.generated.h"


USTRUCT(BlueprintType)
struct FDialogues_Struct
{
	GENERATED_BODY()
		// less the value, more is priority
		UPROPERTY(BlueprintReadWrite)
		int32 Priority = 0;
		// Game time when this info passed to the speaker
		UPROPERTY(BlueprintReadWrite)
		float Time = 0.0f;
		// How long speaker will keep this info as relevant
		UPROPERTY(BlueprintReadWrite)
		float DurationInMemory = 0.0f;
		// witness / ie: I've seen or I heard -- 0 = witness, the more th value the farthest the speaker is from th original
		UPROPERTY(BlueprintReadWrite)
		int32 WitnessLevel = 0;
		// the actual sentence
		UPROPERTY(BlueprintReadWrite)
		FText Sentence = FText::FromString("Hail!");
		// Where this info is related to
		UPROPERTY(BlueprintReadWrite)
		FVector Site = FVector(0);
		// The name of th site where the info comes from
		UPROPERTY(BlueprintReadWrite)
		FString SiteName = FString("Home");
		// Should the speaker point at site location
		UPROPERTY(BlueprintReadWrite)
		bool bPointAt = false;
		// All the people this speaker has already told about this particular info
		UPROPERTY(BlueprintReadWrite)
		TArray<class ACharacterV2*> MarkedCharacters;

		// support for TArray.Sort() -- we sort on Priority
		FORCEINLINE bool operator<(const FDialogues_Struct& V) const
		{
			return Priority < V.Priority;
		}

		// support for TArray.Find() -- 
		FORCEINLINE bool operator==(const FDialogues_Struct& Other) const
		{
			if (Site != Other.Site) return false;
			return true;
		}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HS_API UDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDialogueComponent();

	// Handle how long NPC talks to each others
	FTimerHandle ConversationTimerHandle;
	FTimerDelegate ConversationTimerDelegate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "HS Parameters|Dialogues")
		TArray<FDialogues_Struct> DialogArray;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|Dialogues")
		class UAnimMontage* PointAtMontage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|Dialogues")
		FString DefaultDialogueSentence;

	FDialogues_Struct MakeDialogueStruct(int32 Priority, float Time, float DurationInMemory, int32 WitnessLevel,
		FText Sentence, FVector Site, FString SiteName, bool bPointAt, TArray<ACharacterV2*> MarkedCharacters);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "HS Parameters|Dialogues")
		class USphereComponent* DialogueTrigger;

	UFUNCTION()
		void EndNPCDialogue();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Dialogues")
		FDialogues_Struct ChooseDialogue();

	void PassInfoToSpeaker(FDialogues_Struct InfoToPass, AActor* OtherSpeaker);

	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapDialogueBegin(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapDialogueEnd(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

private:
	class ACharacterV2* OwnerActor;
	bool bCurrentSpeakerIsPlayer;

	void MarkCharacter(int32 IndexToMark);
};
