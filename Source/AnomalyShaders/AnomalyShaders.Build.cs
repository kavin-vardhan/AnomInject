using UnrealBuildTool;

public class AnomalyShaders : ModuleRules
{
	public AnomalyShaders(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		AnomalyEngineRules.Configure(this, Target);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core"
		});

		PublicDefinitions.Add("ANOMALY_SHADERS425=" + (AnomalyEngineRules.Is425(Target) ? "1" : "0"));
		if ((AnomalyEngineRules.Is51(Target) || AnomalyEngineRules.Is425(Target)) && Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PublicDefinitions.Add("ANOMALY_SHADERS=1");
            if (AnomalyEngineRules.Is425(Target)) { PublicDependencyModuleNames.Add("Renderer"); }

			PublicDependencyModuleNames.AddRange(new string[]
			{
				"Engine",
				"RenderCore",
				"RHI"
			});

			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"CoreUObject",
				"Projects"
			});
		}
		else
		{
			PublicDefinitions.Add("ANOMALY_SHADERS=0");
		}
	}
}
