// SillikOne, UE4.


#include "GameMenu.h"
#include "MenuInterface.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"



bool UGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!MenuSwitch) { return false; }

	if (!QuitBtn) { return false; }
	QuitBtn->OnClicked.AddDynamic(this, &UGameMenu::OpenPopupMenu);
	if (!BackBtn) { return false; }
	BackBtn->OnClicked.AddDynamic(this, &UGameMenu::Back);
	if (!QuitConfirmBtn) { return false; }
	QuitConfirmBtn->OnClicked.AddDynamic(this, &UGameMenu::Quit);
	if (!BackConfirmBtn) { return false; }
	BackConfirmBtn->OnClicked.AddDynamic(this, &UGameMenu::Back);
	return true;
}

void UGameMenu::Quit()
{
	// MenuInterface is set in Parent (Menu.cpp)
	if (!MenuInterface) { return; }
	TearDown();
	MenuInterface->QuitToLobby();
}

void UGameMenu::Back()
{
	TearDown();
}

void UGameMenu::OpenPopupMenu()
{
	MenuSwitch->SetActiveWidget(PopupMenu);
}
