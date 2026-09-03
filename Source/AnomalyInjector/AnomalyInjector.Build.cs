using UnrealBuildTool;

public class AnomalyInjector : ModuleRules
{
	public AnomalyInjector(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Foliage"
		});

		if (Target.Version.MajorVersion < 5)
		{
			PublicDefinitions.Add("ANOMALY_UE4_TICKABLE_WORLD_SUBSYSTEM=1");
			PublicDefinitions.Add("ANOMALY_HAS_CONTACT_SHADOW=0");
		}
		else
		{
			PublicDefinitions.Add("ANOMALY_HAS_CONTACT_SHADOW=1");
		}
	}
}
