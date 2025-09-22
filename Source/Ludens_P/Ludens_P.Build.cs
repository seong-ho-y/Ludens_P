using UnrealBuildTool;

public class Ludens_P : ModuleRules
{
    public Ludens_P(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

<<<<<<< Updated upstream
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
=======
        // 여기에 '항상' 필요한 공개 모듈들
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",            // UUserWidget 등
            "EnhancedInput",  // 프로젝트에 Enhanced Input 사용 흔적 있음
        });

        // 링크/위젯 지오메트리 판정용 (IsUnderLocation 등)
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore"
        });

        // 필요시 추가(주석 해제)
        // PublicDependencyModuleNames.Add("GameplayTags");
        // PrivateDependencyModuleNames.AddRange(new string[] { "AIModule" });
    }
>>>>>>> Stashed changes
}
