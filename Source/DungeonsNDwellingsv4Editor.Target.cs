// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DungeonsNDwellingsv4EditorTarget : TargetRules
{
	public DungeonsNDwellingsv4EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("DungeonsNDwellingsv4");
	}
}
