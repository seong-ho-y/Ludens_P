// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Ludens_P : ModuleRules
{
	public Ludens_P(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore",
			"EnhancedInput", 
			"AIModule", "GameplayTasks", 
			"UMG", "Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate", "SlateCore", "ApplicationCore"
		});

		// 👇 에디터 전용 코드: 게임 빌드 시엔 포함되지 않음
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd", "EditorSubsystem"
			});
		}
	}
}
