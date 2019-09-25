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

	UPROPERTY(meta = (BindWidget))
	class UButton* BackBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* BackConfirmBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitConfirmBtn;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitch;

	UPROPERTY(meta = (BindWidget))
	class UWidget* GameMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* PopupMenu;

	UFUNCTION()
	void Quit();
	UFUNCTION()
	void Back();

public:
	UFUNCTION()
	void OpenPopupMenu();
};
