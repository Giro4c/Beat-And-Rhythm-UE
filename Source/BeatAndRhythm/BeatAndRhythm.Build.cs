// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BeatAndRhythm : ModuleRules
{
	public BeatAndRhythm(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"BeatSimple"
		});
	}
}
