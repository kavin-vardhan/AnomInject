# REPORT for 070-02-europa-e1-implement.md
brief: 070-02-europa-e1-implement.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: high
started: 2026-09-03 16:49:18 | finished: 2026-09-03 17:22:51 | duration: 34 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\070-02-europa-e1-implement.live.txt

---- FINAL MESSAGE (Claude Code) ----
Pushed. `europa-e1` = `1f5e305`; `master` untouched at `489c29d`.

---

## 1. VERDICT

> ⛔ **STOPPED — two independent blockers, both measured, both STOPs under the stated failure policy. `E1-G1b` PASSES; the 4.25 gates are unreached.**

Neither blocker is a compile error I could fix. Each needs a decision the approved plan doesn't cover, and each would otherwise require changing the 5.1 delivery build in delivery week.

## 2. BRANCH / HEAD / PUSH

`europa-e1` at **`1f5e305`**, pushed, `origin/europa-e1` == local. **`master` untouched at `489c29d`.** No tag, no merge. Seven owner `CHAT-HANDOFF-*.md` still untracked.

## 3. GATE TABLE

| gate | result | who | evidence |
|---|---|---|---|
| **E1-G1b** | ✅ **PASS** | me | 5.1 `StackOBotEditor` **exit 0**, `StackOBot` **exit 0**. Generated `Definitions.*.h` for non-Shipping Development: **`ANOMALY_CAPTURE 1`**, **`ANOMALY_CONTROL_SERVER 1`**, **`ANOMALY_SHADERS 1`**, `ANOMALY_HAS_CONTACT_SHADOW 1`; `ANOMALY_UE4_TICKABLE_WORLD_SUBSYSTEM` correctly **absent**. **Re-run after the final Build.cs change**, not just once. |
| **E1-G1** (4.25 both targets) | ⛔ **BLOCKED** | me | 4 build attempts; blocked by both blockers below |
| **E1-G0, G2, G3, G4, G5, G6, G7** | ⚪ **UNREACHED** | — | all require a successful 4.25 build |

⚠ **G6's code-fact half is established even so:** `AnomalyCaptureModule.cpp:24` puts `IMPLEMENT_MODULE` outside the guard and the `#else` logs *"compiled out: ANOMALY_CAPTURE=0"*, so the gate is observable, not merely absent. **G7's logging was written and then reverted with the shim** — it lived in the blocked class.

## 4. WHAT SURPRISED ME

**Fixed and moved on (per the failure policy):**

- 🔻 **`GetModuleDirectory` does NOT exist in 4.25's `ModuleRules`.** This **contradicts 070-01 / Phase 0 §5**, which predicted it does. Error: `AnomalyCapture.Build.cs(358,41): error CS0103`. The deeper lesson is `G233`: **`Build.cs` is C#, so a runtime `if (bEnableCapture)` does not protect an API that doesn't exist** — every branch must compile. Fixed with `#if UE_5_0_OR_LATER`; both engines emit those symbols (`RulesAssembly.cs:206` / `:235,239`).
- **An MSVC toolset's folder name is not its version** — UBT reports `MSVC\14.16.27023 (Version=14.16.27051)`. My 070-01 pin used the folder name and failed. 4.25's own source says Microsoft services these in place (`UEBuildWindows.cs:621`). → `G235`.

**Blocker 1 — conditional UCLASS is impossible (design).** 4.25 has no `UTickableWorldSubsystem`; five classes derive from it. Both mechanisms refused, on the 5.1 build:
- `Error: 'UCLASS' must not be inside preprocessor blocks, except for WITH_EDITORONLY_DATA`
- `Error: Unable to find parent class type ... named 'ANOMALY_TICKABLE_WORLD_SUBSYSTEM'` — UHT does not expand macro base classes.

⇒ The base token must be literal and identical on both engines; the only remaining construction **changes 5.1's subsystem base class** → **policy (iii), absolute**. Also measured en route: the two UHTs treat unknown `#if` *asymmetrically* — 5.1 skips it (`UhtHeaderFileParser.cs:888`), 4.25 **hard-errors** (`HeaderParser.cpp:3517`). → `G232`.

**Blocker 2 — the machine-global compiler pin (environment).** `%APPDATA%\...\BuildConfiguration.xml` pins `CompilerVersion 14.38.33130`, load-bearing for 5.1 (which fails on 14.42). **That file is per-user, not per-engine**, and 14.38 is a VS2022 toolset 4.25 can't use. Measured: it **beats `Target.cs`**; `CompilerVersion` has **no command-line form**; and the engine-scoped `Engine\Saved\...` file **also loses** (`XmlConfig.cs:215` vs `:227`). I wrote that engine-scoped file, proved it inert, and **deleted it again** rather than leave something that looks like it works. → `G234`.

🎯 **Also worth flagging as good news:** the tick anchor needed **no E1 change at all** — `AnomalyCaptureSubsystem.cpp` guards its `OnWorldTickEnd` bindings (`:347` encloses `:349-350`; `:548` encloses `:557,562`), so at `ANOMALY_CAPTURE=0` none is compiled. And 070-01's Part A toolchain verdict otherwise **held**: VS2017 BuildTools was found and selected, the SDK pin accepted, the TICKPIN probe behaved as predicted.

## 5. OWNER ERRANDS

1. **Rule on Blocker 1** (via chat) — the only viable option is a shared shim base on both engines reproducing 5.1's `UTickableWorldSubsystem` verbatim (35 lines, quoted at `WorldSubsystem.cpp:46-80`). It needs an explicit waiver of policy (iii) plus its own re-gate, and **should not happen in delivery week**.
2. **Blocker 2 — move the MSVC 14.38 pin out of `%APPDATA%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml` and into StackOBot's own `Target.cs`.** That scopes it to 5.1 and frees every other engine. I did not do it: it changes the delivery build's configuration in delivery week.
3. **`E1-G3`'s eye half** and **`E1-G7`'s editor leg** — both still pending, both now behind the blockers.

## 6. NEEDS-DECISION

1. 🚨 **Blocker 1: does E1 get a waiver of failure-policy (iii) to change the 5.1 subsystem base class, and if so, when?** Everything else in E1 is done or trivially reachable; this is the whole remaining gate. My recommendation: **not until Bates ships**, then do it as its own change with a 5.1 re-gate (tick order, `m40` alignment, and a `speed_ratio` check), since it touches tick gating — the exact machinery `P9` lived in.
2. ⚠ **Blocker 2: who moves the compiler pin, and is `StackOBot.Target.cs` the right home?** It is a one-line relocation, but it is delivery-week configuration and it is the owner's build.

Phase 0 NEEDS-DECISION 1 (deliverable scope a/b/c) remains deferred and is untouched by any of this.
api-equivalent cost (not billed on Max): $26.65 | turns: 92 | running total across all briefs: $549.93
