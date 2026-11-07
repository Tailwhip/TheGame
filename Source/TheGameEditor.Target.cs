// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TheGameEditorTarget : TargetRules
{
	public TheGameEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;  // NOTE: Change this when upgrading UE!
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;  // NOTE: Change this when upgrading UE!
		ExtraModuleNames.Add("TheGame");
	}
}
