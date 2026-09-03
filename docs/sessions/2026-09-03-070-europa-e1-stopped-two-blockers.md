# SESSION 070 — EUROPA E1: STOPPED ON TWO INDEPENDENT BLOCKERS

**2026-09-03, brief 02. Branch `europa-e1` off `master` (`489c29d`). ⛔ NOT MERGED — Bates ships off
`master` this week and `master` is untouched. NO TAG.**

**Outcome: `E1-G1b` PASSES; `E1-G0..G7` on 4.25 are UNREACHED. Two independent blockers, both measured,
both STOPs under the brief's failure policy.** Neither is a compile error I could fix; each requires a
decision the approved plan does not cover, and one of them would alter the 5.1 delivery build.

---

## 1. WHAT SHIPPED, AND IT IS REAL

Phase A's ratified work landed and is **positively verified inert on 5.1**:

| change | file | effect |
|---|---|---|
| module-availability condition | `AnomalyCapture.Build.cs:340-341` | `bEnableCapture = !Shipping && Target.Version.MajorVersion >= 5` |
| module-availability condition | `AnomalyControlServer.Build.cs` | same shape; `WebSocketNetworking` moved **inside** the enabled branch |
| module-availability condition | `AnomalyShaders.Build.cs` | same shape (`G129` — a global shader is a fatal boot, so it must be stubbed prophylactically) |
| C# preprocessor guard | `AnomalyCapture.Build.cs:358` | `#if UE_5_0_OR_LATER` around `GetModuleDirectory` — see `G233` |
| version defines | `AnomalyInjector.Build.cs` | `ANOMALY_UE4_TICKABLE_WORLD_SUBSYSTEM=1` and `ANOMALY_HAS_CONTACT_SHADOW=0` on UE4 |
| `bCastContactShadow` guard ×3 | `AnomalyHiddenClass.cpp:105,120,165` | `#if ANOMALY_HAS_CONTACT_SHADOW` — the one m45 flag 4.25 lacks |

**Environment set up and verified:** the junction exists and works (§4), and the 4.25 host project is
converted (§5).

🎯 **Step 6 of the plan (the tick anchor) turned out to need NO E1 code change, and that is a measurement,
not an omission.** `AnomalyCaptureSubsystem.cpp` is guarded end to end — its `#if ANOMALY_CAPTURE` at
`:347` encloses the `OnWorldTickEnd` bindings at `:349-350`, and `:548` encloses those at `:557,562`.
With `ANOMALY_CAPTURE=0` **no reference to `OnWorldTickEnd` is compiled at all.** The anchor change is
therefore entirely an **E2** concern and is now pre-declared as gate **`E2-E1`** in
`docs/europa/PHASE1-bench-audit-and-plan.md` §E.2a.

---

## 2. `E1-G1b` — PASS, WITH POSITIVE EVIDENCE

Run twice: once after the Build.cs conditions, and **again after the final `#if UE_5_0_OR_LATER` change**,
because that change touches a file 5.1 builds.

- `StackOBotEditor Win64 Development` → **exit 0**
- `StackOBot Win64 Development` → **exit 0**
- Generated `Definitions.*.h` for the 5.1 **Development** (non-Shipping) build:
  `ANOMALY_CAPTURE 1` · `ANOMALY_CONTROL_SERVER 1` · `ANOMALY_SHADERS 1` · `ANOMALY_HAS_CONTACT_SHADOW 1`
- `ANOMALY_UE4_TICKABLE_WORLD_SUBSYSTEM` — **absent on 5.1**, as intended.

⇒ **The new conditions are proven inert on 5.1 positively, not merely "nothing crashed".**

---

## 3. 🚨 BLOCKER 1 — A UObject BASE CLASS CANNOT BE MADE VERSION-CONDITIONAL (design)

4.25 has **no `UTickableWorldSubsystem`** (its `WorldSubsystem.h` is 24 lines). Five classes derive from
it. The plan called the swap "mechanical". **It is not, and UHT refused both mechanisms in under three
minutes each — on the 5.1 build, exactly where Phase A was designed to catch it.**

1. **UCLASS inside a preprocessor block** (`#ifdef`, chosen deliberately because 4.25's UHT accepts
   `#ifdef` where it hard-errors on unknown `#if`):
   > `AnomalyTickableWorldSubsystem.h(12): Error: 'UCLASS' must not be inside preprocessor blocks, except
   > for WITH_EDITORONLY_DATA`
2. **A macro as the base class:**
   > `AnomalySelectorSubsystem.h(11): Error: Unable to find parent class type for
   > 'UAnomalySelectorSubsystem' named 'ANOMALY_TICKABLE_WORLD_SUBSYSTEM'`

⇒ **The base-class token must be literal and identical on both engines.** The only remaining construction
is a shim class named the same on both — which **changes 5.1's subsystem base class**, i.e. shared logic
governing tick gating and creation. **That is failure-policy (iii), which is absolute: stop even if the
change looks obviously safe.** ⇒ **STOPPED.**

📌 Recorded as **`G232`**, together with a second measured asymmetry found on the way: 5.1's UHT
**skips** an unknown `#if` (`UhtHeaderFileParser.cs:888`) while 4.25's UHT makes it a **hard error**
(`HeaderParser.cpp:3517`). Consequence for the port: **on 4.25 no header containing a reflected type may
use `#if <anything>`** beyond `WITH_EDITOR` / `WITH_EDITORONLY_DATA` / `1` / `!CPP`. The existing capture
headers are safe only because they declare no `UCLASS` and so are never handed to UHT.

**Options for chat, costed:**
| option | 5.1 impact | note |
|---|---|---|
| **A. Shared shim base on both engines**, reproducing 5.1's `UTickableWorldSubsystem` verbatim (35 lines, quoted in full in `WorldSubsystem.cpp:46-80`) | 🚨 changes the base class of 5 subsystems | the only *small* option; needs an explicit waiver of (iii) and its own re-gate, and **not in delivery week** |
| **B. Fork the three injector subsystems into engine-specific source files** | none | duplicated UCLASS declarations across engines ⇒ two files to keep in step; and both would still be scanned by UHT on both engines, so it needs the same trick and probably does not work |
| **C. Patch `UTickableWorldSubsystem` into the 4.25 engine tree** | none | ⛔ breaks "public UE APIs only" and means patching the client's engine |
| **D. Defer the injector subsystems; port only what does not derive from them** | none | there is nothing meaningful left — all three injector subsystems derive from it |

⇒ **A is the only viable one, and it is a 5.1 change. That is chat's call, not mine.**

---

## 4. 🚨 BLOCKER 2 — THE MACHINE-GLOBAL COMPILER PIN (environment)

Independent of Blocker 1, and it would stop a 4.25 build even with the subsystems solved.

`%APPDATA%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml` carries
`<CompilerVersion>14.38.33130</CompilerVersion>`. Its own comment records why: **UE 5.1 fails to compile
engine `Core` on MSVC 14.42**, so the pin is load-bearing for the delivery build.

🚨 **That file is per-USER and per-MACHINE, not per-engine.** 14.38 is a VS2022 toolset, which 4.25
cannot use, so it breaks every 4.25 build. **Measured precedence, three legs:**

| override attempt | result |
|---|---|
| `Target.cs` `WindowsPlatform.CompilerVersion` | **LOSES** to the AppData file |
| `-2017` on the command line | **WINS** for `Compiler` only — `CompilerVersion` has no `[CommandLine]` attribute, only `[XmlConfigFile]` |
| `<EngineDir>\Engine\Saved\UnrealBuildTool\BuildConfiguration.xml` — the only engine-scoped location (`XmlConfig.cs:215`) | **LOSES**; AppData is read later (`:227`). Written, tested, **proven inert, and deleted again** rather than left as a file that looks like it does something (`G114`'s shape) |

⇒ **There is no scoped override on 4.25.** The fixes are (a) change the global file — which changes the
5.1 build, **policy (iii)** — or (b) **move the 5.1 pin out of the global file into StackOBot's own
`Target.cs`**, which is where it belongs and which frees the global file for every other engine.
**(b) is correct and I did not do it: it is a change to the delivery build's configuration, in delivery
week, and it is the owner's call.** Recorded as **`G234`**.

---

## 5. ENVIRONMENT ESTABLISHED (both usable regardless of how the blockers are ruled)

**Junction — created by me, no elevation needed, ruling 3's prediction confirmed:**
```
mklink /J "E:\Unreal Projects\StylizedParisStreet\Plugins\AnomalyInjector" ^
          "D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector"
```
Verified by reading `AnomalyInjector.uplugin` **through the 4.25-side path**;
`(Get-Item -Force).LinkType` reads `Junction`, attributes `Directory, ReparsePoint`.
🚨 Its removal footgun is recorded as **`G231`**: `rmdir /S` follows the reparse point and would delete
the entire git working tree. Correct removal is `rmdir` with **no** `/S`, or `fsutil reparsepoint delete`.

**Host project converted** (ruling 7; `StylizedParisStreet.uproject.bak` written first): five files under
`Source/`, plus a `Modules` block in the `.uproject`. Both `Target.cs` carry
`WindowsPlatform.Compiler = VisualStudio2017`, `CompilerVersion = "14.16.27051"`,
`WindowsSdkVersion = "10.0.17763.0"`. ⛔ Nothing else in that folder was touched.

---

## 6. WHAT THE 4.25 BUILD MEASURED BEFORE IT STOPPED — including a refuted Phase 0 prediction

Four 4.25 build attempts were run. Each got further, and each produced a fact:

1. **`GetModuleDirectory` does not exist in 4.25's `ModuleRules`** —
   `AnomalyCapture.Build.cs(358,41): error CS0103`. 🔻 **This REFUTES Phase 0 §5's prediction that
   "`GetModuleDirectory` exists in 4.25".** Fixed with `#if UE_5_0_OR_LATER`; recorded as **`G233`**, whose
   general form is: *a runtime `if` in `Build.cs` does not protect an API that does not exist — C# must
   compile every branch.*
2. **UBT reports `MSVC\14.16.27023 (Version=14.16.27051)`** — the folder name and the version are
   different numbers because Microsoft services toolsets in place (4.25's own source says so at
   `UEBuildWindows.cs:621`). Pinning the folder name fails. Recorded as **`G235`**.
3. **`-2017` is honoured; `CompilerVersion` from AppData is not overridable** → Blocker 2.
4. ✅ **Everything else in the toolchain worked**: 070-01's Part A verdict holds — VS2017 Build Tools 15.9
   is found and selected, the Windows SDK pin is accepted, the plugin's `Build.cs` files compile under
   4.25's UBT, and the TICKPIN probe behaves as predicted.

---

## 7. STATE

- Branch `europa-e1`, pushed. `master` **untouched** at `489c29d`.
- 5.1 both targets green, defines verified.
- 4.25: junction + host project ready; **no successful build**.
- New gotchas **`G231`–`G235`**. E2 entry conditions **`E2-E1`/`E2-E2`/`E2-E3`** pre-declared in the plan
  doc §E.2a.
- ⛔ No tag. ⛔ No merge. ⛔ No artifact produced on either engine.
