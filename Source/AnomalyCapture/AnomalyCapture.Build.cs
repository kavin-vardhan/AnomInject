using UnrealBuildTool;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

public class AnomalyCapture : ModuleRules
{
	private const string ForkSymbol = "sUseFixedGameTickWithVariableRenderTick_Net";
	private const string ForceOnMarkerName = "ANOMINJECT_TICKPIN_FORCE_ON";
	private const string ForceOffMarkerName = "ANOMINJECT_TICKPIN_FORCE_OFF";
	private const int DeepScanFileCap = 20000;
	private const int MaxNameHintsLogged = 8;

	private int ProbeFilesExamined;
	private long ProbeElapsedMs;
	private readonly List<string> ProbeKnownSiteReport = new List<string>();
	private readonly List<string> ProbeNameHits = new List<string>();
	private readonly List<string> ProbePluginDirHits = new List<string>();
	private int ProbeDeepScanFiles;
	private bool bProbeDeepScanTruncated;

	private static string Slash(string InPath)
	{
		return InPath.Replace('\\', '/');
	}

	private bool FileContainsForkSymbol(string FullPath)
	{
		try
		{
			ProbeFilesExamined++;
			return File.ReadAllText(FullPath).Contains(ForkSymbol);
		}
		catch (Exception)
		{
			return false;
		}
	}

	private static bool IsSourceExtension(string FullPath)
	{
		string Ext = Path.GetExtension(FullPath);
		return Ext.Equals(".h", StringComparison.OrdinalIgnoreCase)
			|| Ext.Equals(".cpp", StringComparison.OrdinalIgnoreCase)
			|| Ext.Equals(".inl", StringComparison.OrdinalIgnoreCase);
	}

	private string ProbeRouteA_KnownForkSites()
	{
		string[] KnownSites = new string[]
		{
			Path.Combine("Source", "Runtime", "Core", "Public", "Misc", "App.h"),
			Path.Combine("Source", "Runtime", "Core", "Private", "Misc", "App.cpp"),
			Path.Combine("Source", "Runtime", "Engine", "Private", "GameEngine.cpp"),
			Path.Combine("Source", "Editor", "UnrealEd", "Private", "EditorEngine.cpp")
		};

		string Found = null;
		foreach (string Rel in KnownSites)
		{
			string Full = Path.Combine(EngineDirectory, Rel);
			bool bExists = File.Exists(Full);
			bool bHasSymbol = bExists && FileContainsForkSymbol(Full);
			ProbeKnownSiteReport.Add(string.Format("{0} (exists={1}, symbol={2})", Slash(Rel), bExists, bHasSymbol));
			if (bHasSymbol && Found == null)
			{
				Found = Slash(Rel);
			}
		}
		return Found;
	}

	private string ProbeRouteB_DeepCoreScan()
	{
		string Root = Path.Combine(EngineDirectory, "Source", "Runtime", "Core");
		if (!Directory.Exists(Root))
		{
			return null;
		}
		try
		{
			foreach (string Full in Directory.GetFiles(Root, "*.*", SearchOption.AllDirectories))
			{
				if (!IsSourceExtension(Full))
				{
					continue;
				}
				if (ProbeDeepScanFiles >= DeepScanFileCap)
				{
					bProbeDeepScanTruncated = true;
					return null;
				}
				ProbeDeepScanFiles++;
				if (FileContainsForkSymbol(Full))
				{
					return Slash(Full);
				}
			}
		}
		catch (Exception Ex)
		{
			Console.WriteLine("AnomalyCapture: TICKPIN probe could not scan {0} ({1})", Slash(Root), Ex.Message);
		}
		return null;
	}

	private static bool NameLooksLikeFork(string FullPath)
	{
		string Name = Path.GetFileName(FullPath);
		return Name.StartsWith("FWNet", StringComparison.OrdinalIgnoreCase)
			|| Name.StartsWith("Firewalk", StringComparison.OrdinalIgnoreCase);
	}

	private string ProbeRouteC_ForkNamedFiles()
	{
		string Found = null;

		string[] SourceRoots = new string[]
		{
			Path.Combine(EngineDirectory, "Source", "Runtime"),
			Path.Combine(EngineDirectory, "Source", "Editor")
		};
		foreach (string Root in SourceRoots)
		{
			if (!Directory.Exists(Root))
			{
				continue;
			}
			try
			{
				foreach (string Full in Directory.EnumerateFiles(Root, "*", SearchOption.AllDirectories))
				{
					if (!IsSourceExtension(Full) || !NameLooksLikeFork(Full))
					{
						continue;
					}
					ProbeNameHits.Add(Slash(Full));
					if (FileContainsForkSymbol(Full) && Found == null)
					{
						Found = Slash(Full);
					}
				}
			}
			catch (Exception Ex)
			{
				Console.WriteLine("AnomalyCapture: TICKPIN probe could not scan {0} ({1})", Slash(Root), Ex.Message);
			}
		}

		foreach (string PluginDir in EnumerateForkNamedPluginDirs())
		{
			ProbePluginDirHits.Add(Slash(PluginDir));
			try
			{
				foreach (string Full in Directory.EnumerateFiles(PluginDir, "*", SearchOption.AllDirectories))
				{
					if (!IsSourceExtension(Full))
					{
						continue;
					}
					ProbeNameHits.Add(Slash(Full));
					if (FileContainsForkSymbol(Full) && Found == null)
					{
						Found = Slash(Full);
					}
				}
			}
			catch (Exception Ex)
			{
				Console.WriteLine("AnomalyCapture: TICKPIN probe could not scan {0} ({1})", Slash(PluginDir), Ex.Message);
			}
		}

		return Found;
	}

	private List<string> EnumerateForkNamedPluginDirs()
	{
		List<string> Hits = new List<string>();
		string PluginsRoot = Path.Combine(EngineDirectory, "Plugins");
		if (!Directory.Exists(PluginsRoot))
		{
			return Hits;
		}
		try
		{
			foreach (string First in Directory.GetDirectories(PluginsRoot, "*", SearchOption.TopDirectoryOnly))
			{
				if (NameLooksLikeFork(First))
				{
					Hits.Add(First);
					continue;
				}
				foreach (string Second in Directory.GetDirectories(First, "*", SearchOption.TopDirectoryOnly))
				{
					if (NameLooksLikeFork(Second))
					{
						Hits.Add(Second);
					}
				}
			}
		}
		catch (Exception Ex)
		{
			Console.WriteLine("AnomalyCapture: TICKPIN probe could not scan {0} ({1})", Slash(PluginsRoot), Ex.Message);
		}
		return Hits;
	}

	private bool ProbeForDecoupledTickFork()
	{
		Stopwatch Timer = Stopwatch.StartNew();

		string Route = null;
		string Where = ProbeRouteA_KnownForkSites();
		if (Where != null)
		{
			Route = "A known fork sites";
		}

		if (Where == null)
		{
			Where = ProbeRouteB_DeepCoreScan();
			if (Where != null)
			{
				Route = "B deep content scan of Source/Runtime/Core";
			}
		}

		if (Where == null)
		{
			Where = ProbeRouteC_ForkNamedFiles();
			if (Where != null)
			{
				Route = "C fork-named files";
			}
		}

		Timer.Stop();
		ProbeElapsedMs = Timer.ElapsedMilliseconds;

		if (Where != null)
		{
			Console.WriteLine("AnomalyCapture: TICKPIN probe FOUND symbol '{0}' in {1} [route {2}] - ANOMINJECT_FW_TICKPIN=1 ({3} files examined, {4} ms)", ForkSymbol, Where, Route, ProbeFilesExamined, ProbeElapsedMs);
			Console.WriteLine("AnomalyCapture: TICKPIN probe   that symbol is exactly what the pin writes to, and stock UE 5.1 does not declare it - present means the fork is here AND the pin will compile");
			return true;
		}

		Console.WriteLine("AnomalyCapture: TICKPIN probe NOT FOUND - symbol '{0}' is absent from this engine tree - ANOMINJECT_FW_TICKPIN=0, the pin compiles out entirely ({1} files examined, {2} ms)", ForkSymbol, ProbeFilesExamined, ProbeElapsedMs);
		Console.WriteLine("AnomalyCapture: TICKPIN probe   engine root: {0}", Slash(EngineDirectory));
		Console.WriteLine("AnomalyCapture: TICKPIN probe   route A known fork sites: {0}", string.Join("; ", ProbeKnownSiteReport.ToArray()));
		Console.WriteLine("AnomalyCapture: TICKPIN probe   route B deep content scan: Source/Runtime/Core ({0} files{1})", ProbeDeepScanFiles, bProbeDeepScanTruncated ? string.Format(", TRUNCATED at the {0}-file cap", DeepScanFileCap) : "");
		if (ProbeNameHits.Count > 0)
		{
			int Shown = Math.Min(ProbeNameHits.Count, MaxNameHintsLogged);
			Console.WriteLine("AnomalyCapture: TICKPIN probe   route C fork-named files: {0} filename hit(s), {1} fork-named plugin dir(s) - NONE contained the symbol, so none decided anything. First {2}: {3}", ProbeNameHits.Count, ProbePluginDirHits.Count, Shown, string.Join("; ", ProbeNameHits.GetRange(0, Shown).ToArray()));
		}
		else
		{
			Console.WriteLine("AnomalyCapture: TICKPIN probe   route C fork-named files: no FWNet*/Firewalk* source file under Source/Runtime or Source/Editor, and no FW*/Firewalk* plugin folder under Plugins");
		}
		Console.WriteLine("AnomalyCapture: TICKPIN probe   NOT searched: Plugins is matched by plugin-folder name only (two levels deep), never walked file by file - a full walk of that tree costs about five seconds per build and cannot hold a declaration of FApp, which route B covers exhaustively");
		Console.WriteLine("AnomalyCapture: TICKPIN probe   to force the pin ON regardless of this result, create an empty file named {0} in {1} (and {2} to force it OFF)", ForceOnMarkerName, Slash(PluginDirectory), ForceOffMarkerName);
		return false;
	}

	private int ReadForcedTickPin(out string Mechanism)
	{
		string OnPath = Path.Combine(PluginDirectory, ForceOnMarkerName);
		string OffPath = Path.Combine(PluginDirectory, ForceOffMarkerName);
		bool bOn = File.Exists(OnPath);
		bool bOff = File.Exists(OffPath);

		if (bOn)
		{
			ExternalDependencies.Add(OnPath);
		}
		if (bOff)
		{
			ExternalDependencies.Add(OffPath);
		}

		if (bOn && bOff)
		{
			Console.WriteLine("AnomalyCapture: TICKPIN override AMBIGUOUS - both {0} and {1} exist in {2}. Taking FORCE OFF, which is the stock-equivalent direction. Delete one of them.", ForceOnMarkerName, ForceOffMarkerName, Slash(PluginDirectory));
			Mechanism = "marker file " + Slash(OffPath);
			return 0;
		}
		if (bOn)
		{
			Mechanism = "marker file " + Slash(OnPath);
			return 1;
		}
		if (bOff)
		{
			Mechanism = "marker file " + Slash(OffPath);
			return 0;
		}
		Mechanism = null;
		return -1;
	}

	public AnomalyCapture(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		bool bProbeFound = ProbeForDecoupledTickFork();

		string OverrideMechanism;
		int Forced = ReadForcedTickPin(out OverrideMechanism);

		bool bTickPin = bProbeFound;
		if (Forced >= 0)
		{
			bTickPin = Forced == 1;
			Console.WriteLine("AnomalyCapture: TICKPIN probe OVERRIDDEN by {0} - ANOMINJECT_FW_TICKPIN={1} (probe result was {2})", OverrideMechanism, Forced, bProbeFound ? "FOUND" : "NOT FOUND");
			if (Forced == 1 && !bProbeFound)
			{
				Console.WriteLine("AnomalyCapture: TICKPIN forced ON while the probe found no '{0}'. If this engine really does not declare that symbol the compile WILL FAIL naming it in AnomalyCaptureSubsystem.cpp - that failure is a VALID DIAGNOSTIC RESULT telling us the symbol name is wrong, not a disaster. Delete {1} to restore the probe.", ForkSymbol, ForceOnMarkerName);
			}
		}

		PrivateDefinitions.Add(bTickPin ? "ANOMINJECT_FW_TICKPIN=1" : "ANOMINJECT_FW_TICKPIN=0");

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
