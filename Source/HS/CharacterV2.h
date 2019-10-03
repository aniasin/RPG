// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Characters/HSCharacterBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/DataTable.h"
#include "CharacterV2.generated.h"

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

	virtual void FinishDying() override;

	////////////////////////////
	// Item interaction
		UFUNCTION(NetMulticast, Unreliable)
			void TakeItem(AActor* ItemToTake);
		UFUNCTION(Server, Unreliable, WithValidation)
			void ServerTakeItem(AActor* ItemToTake);

		void Interaction();

		UFUNCTION(Client, Unreliable)
			void ToggleInteractionWidget(AActor* Item);

	/////////////////////////////////
	// Combat
		UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
		void AttachDetachWeaponR(bool bIsAttaching);
		UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
			void AttachDetachWeaponL(bool bIsAttaching);

		void SwitchCombat();
		UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
		void IsShieldUp(bool bIsShieldUp);
		void AIPerformMeleeAttack();
		void AIPerformShieldUp();

		UFUNCTION(BlueprintImplementableEvent, Category = AICombat, meta = (DisplayName = "AISwitchCombat"))
			void K2_AISwitchCombat();
		UFUNCTION(BlueprintImplementableEvent, Category = AICombat, meta = (DisplayName = "AIPerformMeleeAttack"))
			void K2_AIPerformMeleeAttack();
		UFUNCTION(BlueprintImplementableEvent, Category = AICombat, meta = (DisplayName = "AIPerformShieldUp"))
			void K2_AIPerformShieldUp();

	////////////////////////////
	// AI
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|AI")
			bool bIsCivilian;
	// How long and how far AI will search for player when sight is lost
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|AI")
			float SearchTime;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|AI")
			float SearchRadius = 2000.0f;

		///////////////////////
// Equipment
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|Equipment")
		class AWeapon* WeaponR;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|Equipment")
		class AWeapon* WeaponL;


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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|UI")
		TSubclassOf<class UHSFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
		UHSFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "HS Parameters|UI")
		class UWidgetComponent* UIFloatingStatusBarComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|UI")
		class UUserWidget* InteractionWidget;

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
	bool bCanSee;

	// Interaction
	AActor* CurrentFocusedItem;
};
