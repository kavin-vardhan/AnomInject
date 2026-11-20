# REPORT for 069-11-fix-loganomaly-export-editor-build-gate.md
brief: 069-11-fix-loganomaly-export-editor-build-gate.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: medium
started: 2026-09-03 10:12:00 | finished: 2026-09-03 10:24:52 | duration: 13 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-11-fix-loganomaly-export-editor-build-gate.live.txt

---- FINAL MESSAGE (Claude Code) ----
Fixed, gated, documented and pushed.

---

# SESSION 069, BRIEF 11 — `LogAnomaly` EXPORT FIX

## 1. The linker error (pre-fix, working tree at `8bd32a0`)

```
Module.AnomalyCapture.cpp.obj : error LNK2001: unresolved external symbol
"struct FLogCategoryLogAnomaly LogAnomaly" (?LogAnomaly@@3UFLogCategoryLogAnomaly@@A)
D:\...\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyCapture.dll :
fatal error LNK1120: 1 unresolved externals
```

## 2. Editor build exit codes — the gate proven both ways

| | `StackOBotEditor Win64 Development` |
|---|---|
| **before the fix** (tree at `8bd32a0`) | **exit 6** — `LNK2001` / `LNK1120` above |
| **after the one-line fix** | **exit 0** |

The pre-fix build was run **first, deliberately**, so the new gate has a failing case on record rather than only a passing one. Packaged target also rebuilt: **exit 0**.

**The fix**, exactly as the owner had it:
```cpp
ANOMALYINJECTOR_API DECLARE_LOG_CATEGORY_EXTERN(LogAnomaly, Log, All);
```
`AnomalyCapture.Build.cs:335` already lists `"AnomalyInjector"` — **the dependency was never the problem; symbol visibility was.**

🚨 **Why the bench could not have caught it:** every gate here runs the **packaged Development** build, which is **monolithic** — `MODULE_API` expands to nothing and the symbol never crosses a boundary. The **editor** target is modular, and **it is what the cook runs on** (`G47`, runbook §8.6 step 3.5). So this was on a collision course with the client cook, **five milestones downstream of `m38`**, with every bench gate green the whole way.

## 3. Other cross-module symbols checked

Grep: **79 `ANOMALYINJECTOR_API` occurrences across 10 public headers** — `UAnomalyInjectorSubsystem` and `UAnomalyAutoInjectorSubsystem` carry `class ANOMALYINJECTOR_API`; `AnomalyViewport.h` exports 28 including `IsRenderableComponent`, `GetVisibleRenderableActors`, `GetCensusPrefilterActors`, `ProjectActorBoundsToScreenRect`; `AnomalyDefaults.h` 35; plus `AnomalyArgs`, `AnomalyLod`, `AnomalyTargeting`, `AnomalyCensusProvider`, `AnomalySelectorSubsystem`.

🔑 **The decisive check is the linker, not the grep:** the pre-fix link reported **exactly one** unresolved external and the post-fix link reported **none** ⇒ everything else the capture module touches across the boundary was already exported.

## 4. Exe hash + P-C7

Staged **`0EF535DC`** (241,169,920 B) — **identical size** to its predecessor `AD543F42`, which is archived as `StackOBot.exe.m43-AD543F42` (this fix's A-side). ⚠ The hash moved anyway — `G201`: a hash is not content identity.

**Inertness shown by behaviour.** vs the `m43` control under `P-C7 v2`: `frame_index` Δ **0**, `t` Δ **0**, `view` **identical**, every label field byte-identical **except `anomalies`** — where the only differing sub-field was **`mask_value`**.

**That was controlled, not explained away.** A second leg on the *same* post-fix binary:

| leg | binary | first-event tag set |
|---|---|---|
| `M43G_BASE` | `AD543F42` (pre-fix) | `223,225,226,227,228,254` |
| `M44_EXPORT` | `0EF535DC` (post-fix) | `223,**224**,226,227,228,254` |
| `M44_EXPORT_B` | `0EF535DC` (post-fix) | `223,**225**,226,227,228,254` |

⇒ the **same-binary pair differs by the same 23 rows**, and the post-fix second run **reproduces the pre-fix tag set exactly**. `mask_value` is **run-unique** — stencil tag allocation draws from the shared ledger and depends on census claim ordering. **The export change is inert.**

📌 **Client-facing consequence, now measured:** `mask_value` is stable **within** a session, not **across** sessions — which is exactly why `mask_map.json` is per-session and why the client docs say to key on the value **together with** the event/frame range. That line was written on principle at `m43`; it now has a measurement behind it.

## 5. SHAs

| | |
|---|---|
| `master` HEAD == `origin/master` | **`cdd0c4b`** ✅ |
| fix commit | **`f40fee4`** — `fix(injector): export LogAnomaly across modules (ANOMALYINJECTOR_API) - editor/modular build was broken since m38` |
| docs commit | **`cdd0c4b`** — CLAUDE.md milestone gate template (**both targets, both exit 0**), `PRE-DELIVERY-CHECKLIST` §1 editor-build box, **`G221`**, journal 069 §5 |
| tags | none · container unchanged, no cook |

⚠ Note: a sixth untracked owner handoff (`CHAT-HANDOFF-m41-m43-shipped-delivery-week.md`) appeared during this brief. **Not staged**, as the rule requires.

## 6. NEEDS-DECISION

**None.** One thing worth flagging for the cook: the editor binaries on the bench are now current with the fix, but **the Bates editor build needs a pull** to pick up `f40fee4` — the owner already applied the same one-line change locally there, so the pull should be a no-op or a clean fast-forward; if it conflicts, his local edit and this commit are the same line.

**STOP.**
api-equivalent cost (not billed on Max): $17.35 | turns: 19 | running total across all briefs: $291.99
