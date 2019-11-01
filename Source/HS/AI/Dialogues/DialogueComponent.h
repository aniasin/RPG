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

		UPROPERTY(BlueprintReadWrite)
		int32 Priority = 0;
		UPROPERTY(BlueprintReadWrite)
		float Time = 0.0f;
		UPROPERTY(BlueprintReadWrite)
		float DurationInMemory = 0.0f;
		UPROPERTY(BlueprintReadWrite)
		FText Sentence = FText::FromString("Howdee!");
		UPROPERTY(BlueprintReadWrite)
		FVector Site = FVector(0);
		UPROPERTY(BlueprintReadWrite)
		FString SiteName = FString("Home");
		UPROPERTY(BlueprintReadWrite)
		bool bPointAt = false;
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogues")
		TArray<FDialogues_Struct> DialogArray;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogues")
		class UAnimMontage* PointAtMontage;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Dialogues")
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
};
