// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TheGameTarget : TargetRules
{
	public TheGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;  // NOTE: Change this when upgrading UE!
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;  // NOTE: Change this when upgrading UE!
		ExtraModuleNames.Add("TheGame");
	}
}
