# Staged command-line truncation

The first staged census launch exited 0 and produced 480 images, but had no requested absolute log, no VT count block and only one state fingerprint. It is not a passing census leg. The retained stdout and launch receipt show the partial execution.

The Windows process command line was below Windows' 32767-character limit but exceeded UE 5.1.1's `FCommandLine::MaxCommandLineSize = 16384` (`Runtime/Core/Public/Misc/CommandLine.h:26`). Repeated DEFER tokens placed the missing commands and abslog option beyond that limit.

The revised harness uses the engine's `exec` macro reader (`Runtime/Engine/Private/LocalPlayer.cpp:1500`), an absolute file under this run's own Binaries directory, and a process command line below 16300 characters. Each macro line contains one original command; the schedule and measurement limits are unchanged. The macro contents are hashed in the launch receipt. This changes only the harness; no runtime implementation or gate changes.
