// SillikOne, UE4.


#include "MainMenu.h"
#include "GameFramework/PlayerController.h"
#include "MenuInterface.h"
#include "Components/Button.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!MenuSwitch) { return false; }
	if (!MainMenu) { return false; }
	if (!JoinMenu) { return false; }

	if (!JoinBtn) { return false; }
	JoinBtn->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
	if (!HostBtn) { return false; }
	HostBtn->OnClicked.AddDynamic(this, &UMainMenu::HostServer);
	if (!BackBtn) { return false; }
	BackBtn->OnClicked.AddDynamic(this, &UMainMenu::MenuBack);
	if (!JoinIPBtn) { return false; }
	JoinIPBtn->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);
	if (!QuitGameBtn) { return false; }
	QuitGameBtn->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);
	return true;
}

void UMainMenu::JoinServer()
{
	// MenuInterface is set in Parent (Menu.cpp)
	if (!MenuInterface) { return; }
	const FString& Address = IPAddressField->GetText().ToString();
	MenuInterface->JoinServer(Address);
}

void UMainMenu::HostServer()
{
	if (!MenuInterface) { return; }
	MenuInterface->HostServer();
}

void UMainMenu::OpenJoinMenu()
{
	MenuSwitch->SetActiveWidget(JoinMenu);
}

void UMainMenu::MenuBack()
{
	UWidget* CurrentWidget = MenuSwitch->GetActiveWidget();
	if (CurrentWidget == JoinMenu)
	{
		MenuSwitch->SetActiveWidget(MainMenu);
	}
}

void UMainMenu::QuitGame()
{
	UWorld* World = GetWorld();
	if (!ensure(World)) { return; }
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) { return; }
	if (!MenuInterface) { return; }
	PlayerController->ConsoleCommand("Quit");
}

