// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheGame : ModuleRules
{
	public TheGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", "CoreUObject", "Engine", "InputCore", 
			"EnhancedInput", "LearningAgents", "LearningAgentsTraining",
			"Sockets", "Networking", "MLAdapter", "UMG", "Niagara", "Kismet"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"LearningAgentsTraining", "ChaosVehicles", "Sockets", "Networking"
		});
		
		PublicIncludePaths.AddRange(new string[]
		{
			"C:/Users/Taail/AppData/Local/Programs/Python/Python312/include",
			"C:/Users/Taail/Documents/Unreal Projects/TheGame/Source/TheGame/Public/Base",
			"C:/Users/Taail/Documents/Unreal Projects/TheGame/Source/TheGame/Public/PythonCommunication"
		});
		
		PublicAdditionalLibraries.AddRange(new string[]
		{
			"C:/Users/Taail/AppData/Local/Programs/Python/Python312/libs/python312.lib"
		});
		
		bEnableUndefinedIdentifierWarnings = false;
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
