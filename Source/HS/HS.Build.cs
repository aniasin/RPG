// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HS : ModuleRules
{
	public HS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GameplayAbilities", "AIModule" });
        PublicDependencyModuleNames.AddRange(new string[] {"GameplayTasks", "GameplayTags", "HeadMountedDisplay", "UMG", "Slate", "SlateCore", "NavigationSystem" });
	}
}
