// SillikOne, UE4.


#include "Menu.h"
#include "MenuInterface.h"

void UMenu::SetUp()
{
	this->AddToViewport();

	UWorld* World = GetWorld();
	if (!World) { return; }

	// Set InputMode UI only and show Mouse Cursor
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) { return; }
	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;
}

void UMenu::TearDown()
{
	this->RemoveFromViewport();

	UWorld* World = GetWorld();
	if (!World) { return; }
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) { return; }

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;

}

void UMenu::SetMenuInterface(IMenuInterface* MenuInterfaceToSet)
{
	this->MenuInterface = MenuInterfaceToSet;
}