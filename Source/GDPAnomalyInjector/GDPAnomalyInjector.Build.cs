// Copyright GDP Anomaly Injection Project. All Rights Reserved.

using UnrealBuildTool;

public class GDPAnomalyInjector : ModuleRules
{
	public GDPAnomalyInjector(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// INVARIANT (M0): the plugin must stay game-agnostic. Depend only on engine
		// modules reachable from any UE5 project. Do NOT add the host game module
		// (e.g. "StackOBot") or any game-specific module here. See CLAUDE.md / gotchas.md.
		// M1 reaffirms this: the anomaly registry + three anomalies (incl. time_dilation via
			// UGameplayStatics, which is in Engine) add NO new dependency.
			// M2 reaffirms it again: the A1 component finder (AActor::GetComponents<T>), A3 arg
				// parsing, and the three new anomalies use only Engine/Core types — lights via
				// Components/LightComponent.h, static-mesh LOD via Components/StaticMeshComponent.h +
				// Engine/StaticMesh.h, and camera_clipping drives the near clip through the
				// r.SetNearClipPlane console command (GEngine->Exec) + the GNearClippingPlane global
				// (Core), deliberately AVOIDING a RenderCore dependency (AMB-1 ruling).
				// Later milestones may add: Renderer, RenderCore, RHI, Slate, InputCore.
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
