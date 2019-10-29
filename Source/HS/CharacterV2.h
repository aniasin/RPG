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
	// Dialogue
	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
		void BeginDialogue(AActor* PlayerID);
	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
		void EndDialogue();

	AActor* GetCurrentFocusedActor();
	void SetCurrentFocusedActor(AActor* ActorToFocus);


	////////////////////////////
	// Item interaction
		UFUNCTION(NetMulticast, Unreliable)
			void InteractionValidate(AActor* ActorToInteract);
		UFUNCTION(Server, Unreliable, WithValidation)
			void ServerInteractionValidate(AActor* ActorToInteract);
		UFUNCTION(NetMulticast, Unreliable)
			void DropItem(AActor* ItemToDrop);
		UFUNCTION(Server, Unreliable, WithValidation)
			void ServerDropItem(AActor* ItemToDrop);

		void Interaction();
		void DropEquipment();

		UFUNCTION(Client, Unreliable)
			void ToggleInteractionWidget(AActor* Item);
		UFUNCTION(BlueprintImplementableEvent)
			void K2_ToggleWidget(bool bVisibility);

		UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
			void MulticastPlayMontage(class UAnimMontage* Montage, float PlayRate, FName SectionName);
		UFUNCTION(Server, Unreliable, BlueprintCallable)
			void ServerPlayMontage(class UAnimMontage* Montage, float PlayRate, FName SectionName);

	/////////////////////////////////
	// Combat
		UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
		void AttachDetachWeaponR(bool bIsAttaching);
		UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
		void AttachDetachWeaponL(bool bIsAttaching);

		void SwitchCombat();
		UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
		void IsShieldUp(bool bIsShieldUp);
		void AIPerformMeleeAttack();
		void AIPerformShieldUp();
		void AIPerformDash();

		UFUNCTION(BlueprintImplementableEvent, Category = AICombat, meta = (DisplayName = "AISwitchCombat"))
			void K2_AISwitchCombat();
		UFUNCTION(BlueprintImplementableEvent, Category = AICombat, meta = (DisplayName = "AIPerformMeleeAttack"))
			void K2_AIPerformMeleeAttack();
		UFUNCTION(BlueprintImplementableEvent, Category = AICombat, meta = (DisplayName = "AIPerformShieldUp"))
			void K2_AIPerformShieldUp();
		UFUNCTION(BlueprintImplementableEvent, Category = AICombat, meta = (DisplayName = "AIPerformDash"))
			void K2_AIPerformDash();

	// Rotation Replication
		UFUNCTION(Server, Unreliable, WithValidation)
			void Server_UpdateCharacterRotation(FRotator Rotation);
		UFUNCTION(NetMulticast, Unreliable, WithValidation)
			void MultiCast_UpdateCharacterRotation(FRotator Rotation);

			FRotator CurrentRotation;

	////////////////////////////
	// AI
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|AI")
			bool bIsCivilian;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|AI")
			bool bIsArcher;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|AI")
			bool bIsTwoHander;
	// How long and how far AI will search for player when sight is lost
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|AI")
			float SearchTime;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|AI")
			float SearchRadius = 2000.0f;

	///////////////////////////
	// Equipment
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "HS Parameters|Equipment")
		class AWeapon* WeaponR;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "HS Parameters|Equipment")
		class AWeapon* WeaponL;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|UI")
			class UUserWidget* InteractionWidget;
		UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, Category = "HS Parameters|Equipment")
			class UAnimMontage* MontageLeftHand;
		UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, Category = "HS Parameters|Equipment")
			class UAnimMontage* MontageRightHand;
		UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, Category = "HS Parameters|Equipment")
			class UAnimMontage* MontagePickUp;

		UFUNCTION()
		void TakeWeapon(AWeapon* Weapon);
		
		FTimerDelegate MontageLengthTimerDelegate;
		FTimerHandle MontageLengthTimerHandle;

		float GetWeaponSpeed();


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

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HS Parameters|UI")
		UHSFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HS Parameters|UI")
		TSubclassOf<class UHSFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "HS Parameters|UI")
		class UWidgetComponent* UIFloatingStatusBarComponent;

	FGameplayTag DeadTag;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds)override;

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
	class UInputComponent* InputComponent;
	//////////////////////
	// AI
	bool bCanSee;
	bool bIsSpeaking;

	// Interaction
	AActor* CurrentFocusedActor;

	// Combat
	bool bIsInCombat = false;
};
