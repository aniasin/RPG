// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Characters/HSCharacterBase.h"
#include "GenericTeamAgentInterface.h"
#include "HSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HS_API AHSPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AHSPlayerController();
	//Implement GenericTeamInterface	FGenericTeamId TeamId;

	// Gamepad Detection
		/** Can be called to check and see if the player is currently using a gamepad */
	UPROPERTY(Transient, BlueprintReadOnly)
		bool bIsUsingGamepad;

	/** Specifies whether we set `bIsUsingGamepad` to `false` if we receive no input for a period of time. If set to 'true', GamepadTimeout will control how long
		we need to go without receiving input before we set `bIsUsingGamepad` to `false`. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bResetGamepadDetectionAfterNoInput;

	/** How long we can go without receiving a gamepad input before we assume they've stopped using the gamepad */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bResetGamepadDetectionAfterNoInput"))
		float GamepadTimeout;


	// Overrides
	virtual bool InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad) override;
	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;
	virtual void PlayerTick(float DeltaTime) override;

	void CreateHUD();

	UPROPERTY(EditAnywhere, Category = "HS Parameters|UI")
		TSubclassOf<class UHSDamageTextWidgetComponent> DamageNumberClass;

	class UHSHUDWidget* GetHUD();

	UFUNCTION(Client, Reliable, WithValidation)
		void ShowDamageNumber(float DamageAmount, AHSCharacterBase* TargetCharacter);
	void ShowDamageNumber_Implementation(float DamageAmount, AHSCharacterBase* TargetCharacter);
	bool ShowDamageNumber_Validate(float DamageAmount, AHSCharacterBase* TargetCharacter);

	// Simple way to RPC to the client the countdown until they respawn from the GameMode. Will be latency amount of out sync with the Server.
	UFUNCTION(Client, Reliable, WithValidation)
		void SetRespawnCountdown(float RespawnTimeRemaining);
	void SetRespawnCountdown_Implementation(float RespawnTimeRemaining);
	bool SetRespawnCountdown_Validate(float RespawnTimeRemaining);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HS Parameters|UI")
		TSubclassOf<class UHSHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "HS Parameters|UI")
		class UHSHUDWidget* UIHUDWidget;

	// Gamepad Detection
	/** Common logic needed in both `InputAxis()` and `InputKey()` */
	FORCEINLINE void _UpdateGamepad(bool bGamepad)
	{
		bIsUsingGamepad = bGamepad;
		if (bGamepad)
		{
			LastGamepadInputTime = GetWorld()->TimeSeconds;
		}
	}

	// Used to keep track of when we last saw gamepad input
	UPROPERTY(Transient, BlueprintReadOnly)
		float LastGamepadInputTime;

	// Server only
	virtual void OnPossess(APawn* InPawn) override;private:
};
