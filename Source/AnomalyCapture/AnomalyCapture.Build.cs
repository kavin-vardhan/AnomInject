using UnrealBuildTool;
using System;
using System.IO;

public class AnomalyCapture : ModuleRules
{
	private const string ForkMarkerFile = "FWNetSubsystem.cpp";

	private string ProbeForDecoupledTickFork()
	{
		string[] Direct = new string[]
		{
			Path.Combine("Source", "Runtime", "FirewalkNet", "Private", ForkMarkerFile),
			Path.Combine("Source", "Runtime", "FWNet", "Private", ForkMarkerFile),
			Path.Combine("Source", "Runtime", "FWNetworking", "Private", ForkMarkerFile),
			Path.Combine("Source", "Runtime", "Net", "FWNet", "Private", ForkMarkerFile),
			Path.Combine("Source", "Runtime", "Engine", "Private", ForkMarkerFile),
			Path.Combine("Source", "Runtime", "Engine", "Private", "Net", ForkMarkerFile)
		};
		foreach (string Rel in Direct)
		{
			string Full = Path.Combine(EngineDirectory, Rel);
			if (File.Exists(Full))
			{
				return Full;
			}
		}

		string[] Roots = new string[]
		{
			Path.Combine(EngineDirectory, "Source", "Runtime"),
			Path.Combine(EngineDirectory, "Plugins")
		};
		foreach (string Root in Roots)
		{
			if (!Directory.Exists(Root))
			{
				continue;
			}
			try
			{
				string[] Hits = Directory.GetFiles(Root, ForkMarkerFile, SearchOption.AllDirectories);
				if (Hits.Length > 0)
				{
					return Hits[0];
				}
			}
			catch (Exception Ex)
			{
				Console.WriteLine("AnomalyCapture: TICKPIN probe could not scan {0} ({1})", Root, Ex.Message);
			}
		}
		return null;
	}

	public AnomalyCapture(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		string ForkMarker = ProbeForDecoupledTickFork();
		if (ForkMarker != null)
		{
			PrivateDefinitions.Add("ANOMINJECT_FW_TICKPIN=1");
			Console.WriteLine("AnomalyCapture: TICKPIN probe FOUND the decoupled-tick fork marker at {0} - ANOMINJECT_FW_TICKPIN=1", ForkMarker);
		}
		else
		{
			PrivateDefinitions.Add("ANOMINJECT_FW_TICKPIN=0");
			Console.WriteLine("AnomalyCapture: TICKPIN probe did NOT find {0} under {1} - ANOMINJECT_FW_TICKPIN=0, the pin compiles out entirely", ForkMarkerFile, EngineDirectory);
		}

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
