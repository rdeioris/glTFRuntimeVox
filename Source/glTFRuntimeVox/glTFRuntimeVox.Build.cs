// Copyright 2023, Roberto De Ioris.

using UnrealBuildTool;

public class glTFRuntimeVox : ModuleRules
{
    public glTFRuntimeVox(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
            }
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "glTFRuntime",
                "ColorManagement"
            }
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
			}
            );
    }
}
