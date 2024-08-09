// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpiderManProject : ModuleRules
{
	public SpiderManProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "CableComponent","UMG", "Niagara" });
		
		PrivateDependencyModuleNames.AddRange(new string[] {"CableComponent"});
		PrivateIncludePathModuleNames.AddRange(new string[] { "CableComponent" });
	}
}
