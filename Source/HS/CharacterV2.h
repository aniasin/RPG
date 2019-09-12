// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Characters/HSCharacterBase.h"
#include "CharacterV2.generated.h"

UENUM(BlueprintType)
enum class EStatus : uint8
{
	InPeace UMETA(DisplayName = "In Peace"),
	InCombat UMETA(DisplayName = "In Combat"),
	InAlert UMETA(DisplayName = "In Alert")
};

/**
 * A player or AI controlled hero character.
 */
UCLASS()
class HS_API ACharacterV2 : public AHSCharacterBase
{
	GENERATED_BODY()

public:
	ACharacterV2(const class FObjectInitializer& ObjectInitializer);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	class USpringArmComponent* GetCameraBoom();

	class UCameraComponent* GetFollowCamera();

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|Camera")
		float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|Camera")
		FVector GetStartingCameraBoomLocation();

	class UHSFloatingStatusBarWidget* GetFloatingStatusBar();

	USkeletalMeshComponent* GetGunComponent() const;

	virtual void FinishDying() override;

	////////////////////////////
	// AI //////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HS Parameters|AI")
		class UBehaviorTree* BehaviorTree;

	void SetCanSeePlayer(bool bCanSeePlayer);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|Camera")
		float BaseTurnRate = 45.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|Camera")
		float BaseLookUpRate = 45.0f;

	UPROPERTY(BlueprintReadOnly, Category = "HS Parameters|Camera")
		float StartingCameraBoomArmLength;

	UPROPERTY(BlueprintReadOnly, Category = "HS Parameters|Camera")
		FVector StartingCameraBoomLocation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "HS Parameters|Camera")
		class USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "HS Parameters|Camera")
		class UCameraComponent* FollowCamera;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		USkeletalMeshComponent* GunComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|UI")
		TSubclassOf<class UHSFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
		UHSFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "HS Parameters|UI")
		class UWidgetComponent* UIFloatingStatusBarComponent;

	FGameplayTag DeadTag;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Mouse
	void LookUp(float Value);

	// Gamepad
	void LookUpRate(float Value);

	// Mouse
	void Turn(float Value);

	// Gamepad
	void TurnRate(float Value);

	// Mouse + Gamepad
	void MoveForward(float Value);

	// Mouse + Gamepad
	void MoveRight(float Value);

	// Creates and initializes the floating status bar for heroes.
	// Safe to call many times because it checks to make sure it only executes once.
	UFUNCTION()
		void InitializeFloatingStatusBar();

	// Client only
	virtual void OnRep_PlayerState() override;

private:
	//////////////////////
	// AI
	bool bCanSeePlayer;
};
