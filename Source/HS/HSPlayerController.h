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

	// Server only
	virtual void OnPossess(APawn* InPawn) override;private:
};
