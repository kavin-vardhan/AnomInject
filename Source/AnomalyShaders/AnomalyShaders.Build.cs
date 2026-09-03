using UnrealBuildTool;

public class AnomalyShaders : ModuleRules
{
	public AnomalyShaders(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core"
		});

		bool bEnableShaders = Target.Configuration != UnrealTargetConfiguration.Shipping
			&& Target.Version.MajorVersion >= 5;

		if (bEnableShaders)
		{
			PublicDefinitions.Add("ANOMALY_SHADERS=1");

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
