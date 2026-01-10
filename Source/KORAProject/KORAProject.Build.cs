// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KORAProject : ModuleRules
{
	public KORAProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "KORAProject" });
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			"UMG",
			"Slate",
			"SlateCore",
			"CommonUI",
			"CommonInput",
			"CommonUser",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"ModularGameplay",
			"CommonGame",
			"GameFeatures",
			"ModularGameplayActors",
			"AIModule",
            "HairStrandsCore",
			"Niagara",
            "MassEntity",
            "MassCommon",
            "MassActors",
            "MassSpawner",
            "MassRepresentation",
			"MotionWarping",
			"NavigationSystem",
			"MediaAssets",
			"AudioMixer"
        });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AnimGraphRuntime",
			"GameplayMessageRuntime",
            "SphereReveal",
            "PropertyPath",
			"ModularGameplay"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
