// SillikOne, UE4.

#pragma once

#include "CoreMinimal.h"
#include "Menu.h"
#include "MenuInterface.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UMainMenu : public UMenu
{
	GENERATED_BODY()

	virtual bool Initialize() override;

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* HostBtn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* JoinBtn;
	UPROPERTY(meta = (BlueprintReadOnly, BindWidget))
	class UButton* BackBtn;
	UPROPERTY(meta = (BlueprintReadOnly, BindWidget))
	class UButton* JoinIPBtn;
	UPROPERTY(meta = (BlueprintReadOnly, BindWidget))
	class UButton* QuitGameBtn;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitch;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* MainMenu;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* JoinMenu;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UEditableTextBox* IPAddressField;

	UFUNCTION()
	void JoinServer();
	UFUNCTION()
	void HostServer();
	UFUNCTION()
	void OpenJoinMenu();
	UFUNCTION()
	void MenuBack();
	UFUNCTION()
	void QuitGame();

};
