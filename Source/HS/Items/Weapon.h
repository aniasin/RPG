// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


UCLASS()
class HS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
	// Sets default values for this actor's properties
	AWeapon();
	UPROPERTY(EditAnyWhere, Category = "HS Parameters")
	class USceneComponent* SceneComponent;
	UPROPERTY(EditAnyWhere, Category = "HS Parameters")
		class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnyWhere, Category = "HS Parameters")
		class USphereComponent* SphereComponent;
	UPROPERTY(EditAnyWhere, Category = "HS Parameters")
		class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(EditAnyWhere, Category = "HS Parameters")
		class UTextRenderComponent* TextRender;

	UPROPERTY(EditAnyWhere, Category = "HS Parameters")
		FText ItemName;

public:
	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapBegin(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapEnd(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	virtual void PostInitializeComponents()override;


	// The owner of this
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Equipment)
		AActor* OwnerActor;

	//Item Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		bool bIsLeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float SpeedMultiplier;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds)override;

	void UpdateRenderTextRotation(class ACharacterV2* Character);

private:

	ACharacterV2* CurrentFocusedActor;
};
