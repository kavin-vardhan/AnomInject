#include "AnomalyBenchGate.h"
#include "AnomalyInjectorLog.h"

#include "HAL/IConsoleManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

namespace AnomalyBenchGate
{
	static const TCHAR* const LeverPrefix = TEXT("IAI.Bench.");

	bool IsEnabled()
	{
#if UE_BUILD_SHIPPING
		return false;
#else
		const TCHAR* CommandLine = FCommandLine::Get();
		return FParse::Param(CommandLine, TEXT("IAIBench")) || FParse::Param(CommandLine, TEXT("IAIBenchFixture"));
#endif
	}

	FString DescribeFlags()
	{
		FString Flags;
#if !UE_BUILD_SHIPPING
		const TCHAR* CommandLine = FCommandLine::Get();
		if (FParse::Param(CommandLine, TEXT("IAIBench")))
		{
			Flags += TEXT("-IAIBench");
		}
		if (FParse::Param(CommandLine, TEXT("IAIBenchFixture")))
		{
			Flags += Flags.IsEmpty() ? TEXT("-IAIBenchFixture") : TEXT(" -IAIBenchFixture");
		}
#endif
		return Flags;
	}

	int32 SweepUngatedLevers(const TCHAR* Where)
	{
		TArray<TPair<FString, IConsoleObject*>> Found;
		IConsoleManager::Get().ForEachConsoleObjectThatStartsWith(
			FConsoleObjectVisitor::CreateLambda([&Found](const TCHAR* Name, IConsoleObject* Object)
			{
				if (Object)
				{
					Found.Emplace(FString(Name), Object);
				}
			}),
			LeverPrefix);

		if (IsEnabled())
		{
			UE_LOG(LogAnomaly, Log, TEXT("IAI bench levers: ENABLED (%s) where=%s present=%d"),
				*DescribeFlags(), Where, Found.Num());
			return 0;
		}

		int32 Masked = 0;
		int32 Newly = 0;
		int32 Preset = 0;
		for (const TPair<FString, IConsoleObject*>& Entry : Found)
		{
			IConsoleObject* Object = Entry.Value;
			++Masked;
			if (Object->TestFlags(ECVF_Unregistered))
			{
				continue;
			}
			++Newly;
			if (IConsoleVariable* Variable = Object->AsVariable())
			{
				const uint32 SetBy = (uint32)Variable->GetFlags() & (uint32)ECVF_SetByMask;
				if (SetBy != (uint32)ECVF_SetByConstructor)
				{
					++Preset;
					UE_LOG(LogAnomaly, Warning,
						TEXT("IAI bench lever '%s' was already SET (set-by 0x%08x) before the bench gate masked it. ")
						TEXT("Masking stops the console reaching it, but the value it was given stays in effect. ")
						TEXT("Remove that setting, or launch with -IAIBench to use bench levers."),
						*Entry.Key, SetBy);
				}
			}
			Object->SetFlags((EConsoleVariableFlags)((uint32)Object->GetFlags() | (uint32)ECVF_Unregistered));
		}

		UE_LOG(LogAnomaly, Log, TEXT("IAI bench levers: DISABLED (%d masked) where=%s newly=%d preset=%d"),
			Masked, Where, Newly, Preset);
		return Newly;
	}

	IConsoleCommand* FindLeverCommand(const TCHAR* Name)
	{
		IConsoleObject* Object = IConsoleManager::Get().FindConsoleObject(Name);
		if (!Object || Object->TestFlags(ECVF_Unregistered))
		{
			return nullptr;
		}
		return Object->AsCommand();
	}
}
