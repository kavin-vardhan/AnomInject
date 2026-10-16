using UnrealBuildTool;

public class AnomalyInjector : ModuleRules
{
	public AnomalyInjector(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		AnomalyEngineRules.Configure(this, Target);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Foliage",
			"RenderCore",
			"RHI",
			"AnomalyShaders"
		});
	}
}

public static class AnomalyEngineRules
{
    public static bool Is425(ReadOnlyTargetRules Target)
    {
        return Target.Version.MajorVersion == 4 && Target.Version.MinorVersion == 25;
    }

    public static bool Is51(ReadOnlyTargetRules Target)
    {
        return Target.Version.MajorVersion == 5 && Target.Version.MinorVersion == 1;
    }

    public static void Configure(ModuleRules Rules, ReadOnlyTargetRules Target)
    {
        if (!Is425(Target) && !Is51(Target))
        {
            throw new BuildException("AnomalyInjector supports exactly Unreal Engine 4.25 and 5.1.");
        }
        if (Is425(Target))
        {
            Rules.CppStandard = CppStandardVersion.Cpp17;
        }
        bool bBench = Is51(Target) && Target.Configuration != UnrealTargetConfiguration.Shipping
            && (Target.Type == TargetType.Game || Target.Type == TargetType.Editor);
        Rules.PublicDefinitions.Add(bBench ? "ANOMALY_PROFILE_BENCH=1" : "ANOMALY_PROFILE_BENCH=0");
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            System.Console.WriteLine("Anomaly profile: engine={0}.{1} compiler={2} version={3} SDK={4} cppStandard={5}",
                Target.Version.MajorVersion, Target.Version.MinorVersion,
                Target.WindowsPlatform.Compiler, Target.WindowsPlatform.CompilerVersion,
                Target.WindowsPlatform.WindowsSdkVersion, Rules.CppStandard);
        }
    }
}
