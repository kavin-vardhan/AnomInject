using UnrealBuildTool;

public class AnomalyControlServer : ModuleRules
{
    public AnomalyControlServer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        AnomalyEngineRules.Configure(this, Target);
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "AnomalyInjector" });
        bool bEnabled = (AnomalyEngineRules.Is425(Target) || AnomalyEngineRules.Is51(Target)) && Target.Configuration != UnrealTargetConfiguration.Shipping;
        PublicDefinitions.Add(bEnabled ? "ANOMALY_CONTROL_SERVER=1" : "ANOMALY_CONTROL_SERVER=0");
        if (bEnabled)
        {
            PrivateDependencyModuleNames.AddRange(new[] { "AnomalyCapture", "Json" });
            if (AnomalyEngineRules.Is51(Target)) { PrivateDependencyModuleNames.Add("WebSocketNetworking"); }
            else
            {
                PrivateDependencyModuleNames.Add("Sockets");
                PublicSystemLibraries.Add("Iphlpapi.lib");
                AddEngineThirdPartyPrivateStaticDependencies(Target, "libWebSockets", "OpenSSL", "zlib");
            }
        }
    }
}
