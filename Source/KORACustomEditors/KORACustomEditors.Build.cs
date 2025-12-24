using UnrealBuildTool;

public class KORACustomEditors: ModuleRules
{
    public KORACustomEditors(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core", 
            "CoreUObject", 
            "Engine", 
            "KORAProject",
            "DirectoryWatcher",
            "KRUtilManager"
            //"EditorScriptingUtilities" 
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "UnrealEd",
            "Slate",
            "SlateCore",
            "AssetTools",
            "DeveloperSettings",
            "Json",
            "JsonUtilities",
            "Blutility",
            "EditorSubsystem",
            "EditorWidgets",
            "Projects",
            "DesktopWidgets",
            "EditorScriptingUtilities",
            "ToolMenus",
            "AssetRegistry",
            "UMG",
            "GameplayTags",
            "KORAProject",
            "DirectoryWatcher",
            "KRUtilManager"
        });
    }
}
