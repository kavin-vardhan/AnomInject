using UnrealBuildTool;

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

			// Stage 1: async backbuffer color readback (OnBackBufferReadyToPresent + viewport-rect
			// clip + FRHIGPUTextureReadback). Renderer / Renderer-private (FScreenPassTexture /
			// FPostProcessMaterialInputs) are deferred to Stage 3 (the stencil/depth SVE).
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"RenderCore",
				"RHI",
				"Slate",
				"SlateCore",
				"ApplicationCore"
			});
		}
		else
		{
			PublicDefinitions.Add("ANOMALY_CAPTURE=0");
		}

		// Editor-only: suppress the PIE "Shift+F1 for Mouse Cursor" hint during a capture run
		// (ULevelEditorPlaySettings). WITH_EDITOR-guarded; absent from packaged builds.
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
	}
}
