using UnrealBuildTool;

public class AnomalyBench : ModuleRules
{
    public AnomalyBench(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        AnomalyEngineRules.Configure(this, Target);
        bool bEnabled = AnomalyEngineRules.Is51(Target) && Target.Configuration != UnrealTargetConfiguration.Shipping
            && (Target.Type == TargetType.Game || Target.Type == TargetType.Editor);
        PublicDefinitions.Add(bEnabled ? "ANOMALY_BENCH=1" : "ANOMALY_BENCH=0");
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine" });
        if (bEnabled)
        {
            PrivateDependencyModuleNames.AddRange(new[] { "AnomalyInjector", "AnomalyCapture" });
        }
    }
}
