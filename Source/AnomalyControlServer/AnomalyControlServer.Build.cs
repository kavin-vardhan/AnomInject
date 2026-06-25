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
			"WebSocketNetworking",
			"Json",
			"ImageWrapper"
		});

		if (Target.Configuration == UnrealTargetConfiguration.Shipping)
		{
			PublicDefinitions.Add("ANOMALY_CONTROL_SERVER=0");
		}
		else
		{
			PublicDefinitions.Add("ANOMALY_CONTROL_SERVER=1");
		}
	}
}
