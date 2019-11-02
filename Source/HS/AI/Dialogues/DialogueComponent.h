// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Internationalization/Text.h"
#include "DialogueComponent.generated.h"


USTRUCT(BlueprintType)
struct FDialogues_Struct
{
	GENERATED_BODY()
		// 0 = top priority
		UPROPERTY(BlueprintReadWrite)
		int32 Priority = 0;
		UPROPERTY(BlueprintReadWrite)
		float Time = 0.0f;
		UPROPERTY(BlueprintReadWrite)
		float DurationInMemory = 0.0f;
		UPROPERTY(BlueprintReadWrite)
		FText Sentence = FText::FromString("Hail!");
		UPROPERTY(BlueprintReadWrite)
		FVector Site = FVector(0);
		UPROPERTY(BlueprintReadWrite)
		FString SiteName = FString("Home");
		UPROPERTY(BlueprintReadWrite)
		bool bPointAt = false;
		UPROPERTY(BlueprintReadWrite)
		TArray<ACharacterV2*> MarkedCharacters;

		// support for TArray.Sort() -- we sort on Priority
		FORCEINLINE bool operator<(const FDialogues_Struct& V) const
		{
			return Priority < V.Priority;
		}

		// support for TArray.Find()
		FORCEINLINE bool operator==(const FDialogues_Struct& Other) const
		{
			if (Time != Other.Time) return false;
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|Dialogues")
		TArray<FDialogues_Struct> DialogArray;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|Dialogues")
		class UAnimMontage* PointAtMontage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|Dialogues")
		FString DefaultDialogueSentence;

	FDialogues_Struct MakeDialogueStruct(int32 Priority, float Time, float DurationInMemory, 
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

	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapDialogueBegin(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapDialogueEnd(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

private:
	class ACharacterV2* OwnerActor;

	void MarkCharacter(int32 IndexToMark);
};
