// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include "HSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UHSGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
	
public:
	UHSGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init();

	UFUNCTION()
		virtual void BeginLoadingScreen(const FString& MapName);
	UFUNCTION()
		virtual void EndLoadingScreen(UWorld* InLoadedWorld);

	UFUNCTION(BlueprintCallable, Category = "MenuSystem")
		void LoadMainMenu();
	UFUNCTION(BlueprintCallable, Category = "MenuSystem")
		void LoadGameMenu();

	UFUNCTION()
		virtual void HostServer()override;

	UFUNCTION()
		virtual void JoinServer(const FString Address)override;

	UFUNCTION()
		virtual void QuitToLobby()override;

protected:
	TSubclassOf<class UUserWidget> MainMenuWidget;
	TSubclassOf<class UUserWidget> GameMenuWidget;

	class UMainMenu* MainMenu;
	class UGameMenu* GameMenu;
};
