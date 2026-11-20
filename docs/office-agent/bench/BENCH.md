# The home bench, in one page

**What it is.** CaptureBench is a separate, non-shipping UE plugin (its own local git repo, no remote)
that sits beside AnomalyInjector in the StackOBot host on the home PC. It holds the bench fixtures and,
above all, the Python/PowerShell **instruments** that run and judge capture legs. Nothing in it ever
ships to the client. `capturebench/` here is a text snapshot of its tools and documents on 2026-11-20
(tracked and untracked files; the codename scrub verifier itself is left out on purpose because it
carries the term list). `capturebench-branches.txt` and `capturebench-log.txt` give its branches,
untracked files and full log. Cooked content and binaries are not included.

**The fixtures.** `CB_GateLevel` (a synthetic, static, settled-camera level in StackOBot, cooked into a
bench-only package, never delivered), `MainWorld` (StackOBot's real map, with motion), Lyra
(`L_ShooterGym`, a second UE 5.1 fixture, mounted as a detached git worktree), test levels for LOD,
texture corruption and terrain, and on the 4.25 side a marketplace street scene standing in for Europa.

**How a leg runs.** `tools/run_leg.ps1` launches the packaged bench build with `-ExecCmds` (startup
console commands), forces focus, waits for `run_summary.json`, then copies the session to the bank.
The packaged-leg recipe is `docs/setup-runbook.md` §8. A leg is **valid** only if its preconditions
hold (pose matched, focus matched, enough counted events, no foreign load); an invalid leg is re-run,
never averaged in, and invalidity is decided by a rule fixed **before** the result is read (A63).

**How a gate is judged.**
1. Predictions are frozen in writing before any leg (`docs/predictions/`, `runs/`).
2. The instrument is proven against a known answer first (A53) and a **can-fail** lever shows it can
   fire (G96): e.g. `IAI.Bench.SynthTickOrder`, `IAI.Bench.HideOmitDepthPassSilencing`,
   `IAI.Bench.VetoArmUngated`, `IAI.Bench.StuckMipNoHold`.
3. Alignment gates run in **both tick orders** (native and `IAI.Bench.SynthTickOrder`); pixel comparisons
   run at the **AA-off arbiter** configuration in native order, and count only if the control pair reads 0.
4. Cross-binary comparisons use a **control pair** (two runs of one binary) to measure the run-unique
   field set; the test pair's differences must be a subset of it.
5. Every build is identified by the exe hash **and** the cooked container (G121), and new strings are read
   back from the staged binary in both encodings (A44).
6. Both targets build with 0 errors (packaged monolithic and Editor modular, G221).

**Permanent gates** (see `CLAUDE.md`): ONSET (first mask = first label = first changed picture, per
type, both orders) and MASK-PICTURE-PAIRING (`NEITHER == 0` and `PREVIOUS == 0`).

**Why the office matters.** The bench is one machine with a handful of fixtures. Several of the worst
defects (m17, m31, m35, P9) appeared only on a real host. Certification depth on the bench says nothing
about a second host.

**Code files in this pack carry an added `.txt` suffix** (for example `run_leg.ps1.txt`): they are a text snapshot, not runnable copies, and the repo's no-comments rule for source does not apply to them.
