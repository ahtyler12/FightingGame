// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FightingGame : ModuleRules
{
	public FightingGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils",  "ApplicationCore", "Networking" });
        PrivateDependencyModuleNames.AddRange(new string[] { "GGPOUE4" });
    }
}
