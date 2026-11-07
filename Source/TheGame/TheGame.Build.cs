// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class TheGame : ModuleRules
{
	public TheGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", "CoreUObject", "Engine", "InputCore", 
			"EnhancedInput", "Sockets", "Networking", "UMG", "Niagara", "Kismet"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Sockets", "Networking"
		});
		
    string pyVersion = "314"; // NOTE: Change this when upgrading Python!
    string moduleDir = ModuleDirectory;
    string publicDir = moduleDir + "\\Public";
    string localDir = Environment.GetFolderPath(
        Environment.SpecialFolder.LocalApplicationData
    );
    string pythonDir = localDir + "\\Programs\\Python\\Python" + pyVersion + "\\";
    Console.WriteLine("moduleDir: " + moduleDir);
    Console.WriteLine("publicDir: " + publicDir);
    Console.WriteLine("localDir: " + localDir);
    Console.WriteLine("pythonDir: " + pythonDir);
		PublicIncludePaths.AddRange(new string[]
		{
			publicDir,
			pythonDir + "\\include"
    });
		
		PublicAdditionalLibraries.AddRange(new string[]
		{
			pythonDir + "\\libs\\python" + pyVersion + ".lib"
		});
		
		bEnableUndefinedIdentifierWarnings = false;
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
