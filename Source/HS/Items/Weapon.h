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

public:
	// The owner of this
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Equipment)
		AActor* OwnerActor;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HS Parameters")
		TArray<class ACharacterV2*> CurrentOverlapingActors;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HS Parameters")
		FText Name;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HS Parameters")
	class USceneComponent* SceneComponent;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HS Parameters")
		class UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HS Parameters")
		class USphereComponent* SphereComponent;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HS Parameters")
		class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HS Parameters")
		class UAnimMontage* MontageRightHand;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HS Parameters")
		class UAnimMontage* MontageLeftHand;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "HS Parameters")
		class UAnimMontage* MontageHit;

	UPROPERTY(EditAnyWhere, Category = "HS Parameters")
		FText ItemName;

	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapBegin(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapEnd(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	virtual void PostInitializeComponents()override;

	//Item Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		bool bIsLeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float SpeedMultiplier;

	// Interactions
	void ItemTaken();
	void ItemDropped();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds)override;

};
