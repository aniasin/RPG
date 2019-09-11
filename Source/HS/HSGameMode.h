// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HSGameMode.generated.h"

UCLASS(minimalapi)
class AHSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHSGameMode();

	void HeroDied(AController* Controller);

protected:
	float RespawnDelay;

	TSubclassOf<class ACharacterV2> HeroClass;

	AActor* EnemySpawnPoint;

	virtual void BeginPlay() override;

	void RespawnHero(AController* Controller);
};



