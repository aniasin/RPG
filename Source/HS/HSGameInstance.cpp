// SillikOne.


#include "HSGameInstance.h"
#include "Classes/Engine/Player.h"
#include "GameFramework/PlayerController.h"
#include "Classes/GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "MenuSystem/Menu.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/GameMenu.h"
#include "UI/HSGameViewportClient.h"
#include "Classes/Engine/Console.h"
#include "Runtime/MoviePlayer/Public/MoviePlayer.h"

UHSGameInstance::UHSGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetClass(TEXT("/Game/MenuSystem/W_MainMenu"));
	if (!ensure(MainMenuWidgetClass.Class)) { return; }
	MainMenuWidget = MainMenuWidgetClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> GameMenuWidgetClass(TEXT("/Game/MenuSystem/W_GameMenu"));
	if (!ensure(GameMenuWidgetClass.Class)) { return; }
	GameMenuWidget = GameMenuWidgetClass.Class;
}

void UHSGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UHSGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UHSGameInstance::EndLoadingScreen);

	UE_LOG(LogTemp, Warning, TEXT("Found Class: %s"), *MainMenuWidget->GetName())
	UE_LOG(LogTemp, Warning, TEXT("Found Class: %s"), *GameMenuWidget->GetName())
}

void UHSGameInstance::BeginLoadingScreen(const FString& MapName)
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
}

void UHSGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	const UWorld* World = GetWorld();
	if (World)
	{
		UHSGameViewportClient* GameViewportClient = Cast<UHSGameViewportClient>(World->GetGameViewport());
		if (GameViewportClient)
		{
			GameViewportClient->Fade(2.0f, false);
		}
	}
}

void UHSGameInstance::LoadMainMenu()
{
	// Create and show Menu
	if (!MainMenuWidget) { return; }
	MainMenu = CreateWidget<UMainMenu>(this, MainMenuWidget);
	MainMenu->SetUp();
	MainMenu->SetMenuInterface(this);
}

void UHSGameInstance::LoadGameMenu()
{
	// Create and show Menu
	if (!GameMenuWidget) { return; }
	GameMenu = CreateWidget<UGameMenu>(this, GameMenuWidget);
	GameMenu->SetUp();
	GameMenu->SetMenuInterface(this);
}

void UHSGameInstance::HostServer()
{
	if (MainMenu) { MainMenu->TearDown(); }

	UEngine* Engine = GetEngine();
	if (!ensure(Engine)) { return; }
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting!"));

	UWorld* World = GetWorld();
	if (!ensure(World)) { return; }
	World->ServerTravel("/Game/Maps/Map_01?Listen");
}

void UHSGameInstance::JoinServer(const FString Address)
{
	if (MainMenu) { MainMenu->TearDown(); }
	UEngine* Engine = GetEngine();
	if (!ensure(Engine)) { return; }
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Joining: %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController)) { return; }

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UHSGameInstance::QuitToLobby()
{
	if (GameMenu) { GameMenu->TearDown(); }
	UEngine* Engine = GetEngine();
	if (!ensure(Engine)) { return; }
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Leaving...")));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController)) { return; }

	UWorld* World = GetWorld();
	if (!ensure(World)) { return; }
	UHSGameViewportClient* GameViewportClient = Cast<UHSGameViewportClient>(World->GetGameViewport());
	if (!GameViewportClient) { return; }
	GameViewportClient->Fade(1.0, true);

	// if Server
	if (PlayerController->GetCharacter()->HasAuthority())
	{
		World->ServerTravel("/Game/Maps/MainMenu");
		return;
	}
	PlayerController->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
}
