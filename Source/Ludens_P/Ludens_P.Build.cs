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
        // ���⿡ '�׻�' �ʿ��� ���� ����
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",            // UUserWidget ��
            "EnhancedInput",  // ������Ʈ�� Enhanced Input ��� ���� ����
        });

        // ��ũ/���� ������Ʈ�� ������ (IsUnderLocation ��)
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore"
        });

        // �ʿ�� �߰�(�ּ� ����)
        // PublicDependencyModuleNames.Add("GameplayTags");
        // PrivateDependencyModuleNames.AddRange(new string[] { "AIModule" });
    }
>>>>>>> Stashed changes
}
