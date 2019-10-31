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
		int32 Priority;
		UPROPERTY(BlueprintReadWrite)
		float Time;
		UPROPERTY(BlueprintReadWrite)
		float DurationInMemory;
		UPROPERTY(BlueprintReadWrite)
		FString Sentence;
		UPROPERTY(BlueprintReadWrite)
		FVector Site;
		UPROPERTY(BlueprintReadWrite)
		FString SiteName;
		UPROPERTY(BlueprintReadWrite)
		bool bPointAt;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HS_API UDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDialogueComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogues")
		TArray<FDialogues_Struct> DialogArray;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogues")
		class UAnimMontage* PointAtMontage;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Dialogues")
		class USphereComponent* DialogueTrigger;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapDialogueBegin(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapDialogueEnd(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

private:
	class ACharacterV2* OwnerActor;
};
