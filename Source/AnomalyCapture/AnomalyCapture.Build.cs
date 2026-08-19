using UnrealBuildTool;
using System.IO;

public class AnomalyCapture : ModuleRules
{
	public AnomalyCapture(ReadOnlyTargetRules Target) : base(Target)
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
			"ImageWrapper",
			"Json"
		});

		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PublicDefinitions.Add("ANOMALY_CAPTURE=1");

			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"RenderCore",
				"RHI",
				"Renderer",
				"Slate",
				"SlateCore",
				"ApplicationCore",
				"AnomalyShaders"
			});

			PrivateIncludePaths.Add(Path.Combine(GetModuleDirectory("Renderer"), "Private"));
		}
		else
		{
			PublicDefinitions.Add("ANOMALY_CAPTURE=0");
		}

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
	}
}
