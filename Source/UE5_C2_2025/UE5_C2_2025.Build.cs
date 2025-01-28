// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE5_C2_2025 : ModuleRules
{
	public UE5_C2_2025(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "GameplayAbilities", "GameplayTags" }) ;
	}
}
