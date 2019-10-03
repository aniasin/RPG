// SillikOne, UE4.

#pragma once

#include "CoreMinimal.h"
#include "Menu.h"
#include "MenuInterface.h"
#include "GameMenu.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UGameMenu : public UMenu
{
	GENERATED_BODY()
	
	virtual bool Initialize() override;
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BackBtn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* QuitBtn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* BackConfirmBtn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* QuitConfirmBtn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* SettingsBtn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* SettingsBackBtn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* SettingsResetBtn;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* SettingsSaveBtn;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitch;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* GameMenu;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* SettingsMenu;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidget* PopupMenu;

	UFUNCTION()
	void Quit();
	UFUNCTION()
	void Back();

public:
	UFUNCTION()
	void OpenPopupMenu();
	UFUNCTION()
		void Settings();
};
