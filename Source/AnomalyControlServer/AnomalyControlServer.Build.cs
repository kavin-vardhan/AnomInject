using UnrealBuildTool;

public class AnomalyControlServer : ModuleRules
{
    public AnomalyControlServer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        AnomalyEngineRules.Configure(this, Target);
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "AnomalyInjector" });
        bool bEnabled = AnomalyEngineRules.Is51(Target) && Target.Configuration != UnrealTargetConfiguration.Shipping;
        PublicDefinitions.Add(bEnabled ? "ANOMALY_CONTROL_SERVER=1" : "ANOMALY_CONTROL_SERVER=0");
        if (bEnabled)
        {
            PrivateDependencyModuleNames.AddRange(new[] { "AnomalyCapture", "WebSocketNetworking", "Json" });
        }
    }
}
