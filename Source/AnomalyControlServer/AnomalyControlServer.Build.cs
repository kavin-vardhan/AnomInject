// Copyright GDP Anomaly Injection Project. All Rights Reserved.

using UnrealBuildTool;

/**
 * AnomalyControlServer — the Tier-2 external-control runtime module (Slice 0: transport spike).
 *
 * ISOLATION INVARIANT: this is a SEPARATE runtime module from the core `AnomalyInjector` module
 * precisely so the HTTP/WS/JSON/Image dependencies (and an *Experimental* transport plugin) never
 * leak into the core. The core module stays Core/CoreUObject/Engine/InputCore — untouched. Do NOT
 * move any of these deps into AnomalyInjector.Build.cs.
 *
 * Transport (ratified): a SINGLE WebSocket carries commands + read-back + JPEG preview frames on one
 * port/connection, via WebSocketNetworking's standalone IWebSocketServer (NOT the WebSocket NetDriver).
 */
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
			"AnomalyInjector",      // core plugin module — Slice 1 calls its public Apply/Revert + read-back surface
			"WebSocketNetworking",  // standalone IWebSocketServer (Experimental plugin, Runtime module, Win64 OK)
			"Json",                 // FJsonObject parse/serialize for the control protocol + snapshot
			"ImageWrapper"          // EImageFormat::JPEG encode of the preview frame
			// Slice-1 preview = FViewport::ReadPixels on the game viewport (Engine-only, game-view-only, no disk).
			// The Slice-0 backbuffer path's RHI / RenderCore / Slate / SlateCore deps were DROPPED here; they
			// return only with the deferred render-thread async-readback upgrade (higher fps + packaged capture).
		});

		// Build switch (ratified): the control server compiles OUT of Shipping. Even when compiled in
		// (Editor/Development), the listener is DORMANT by default — started only via IAI.Server.Start.
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
