using UnrealBuildTool;

public class AnomalyControlServer : ModuleRules
{
	public AnomalyControlServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AnomalyInjector",
			"AnomalyCapture",
			"Json"
		});

		bool bEnableControlServer = Target.Configuration != UnrealTargetConfiguration.Shipping
			&& Target.Version.MajorVersion >= 5;

		if (bEnableControlServer)
		{
			PublicDefinitions.Add("ANOMALY_CONTROL_SERVER=1");

			PrivateDependencyModuleNames.Add("WebSocketNetworking");
		}
		else
		{
			PublicDefinitions.Add("ANOMALY_CONTROL_SERVER=0");
		}
	}
}
