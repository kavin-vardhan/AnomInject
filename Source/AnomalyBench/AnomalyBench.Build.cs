using UnrealBuildTool;
public class AnomalyBench : ModuleRules
{
    public AnomalyBench(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "AnomalyInjector", "AnomalyCapture" });
    }
}
