---
name: g121-build-identity-exe-plus-pak
description: "A content-only re-cook leaves the exe hash unchanged, so the exe hash identifies only half the build — record exe hash AND pak identity"
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-08-19T11:38:24.063Z
---

**The MainWorld + G118 re-cook (2026-08-19) changed what the build contains and what it enforces, and
left the binary byte-identical.**

| | before | after |
|---|---|---|
| exe SHA-256 / mtime | `101AFEA4` / 12:32:39 | **`101AFEA4` / 12:32:39 — identical** |
| cooked maps | `CB_GateLevel`, `Entry`, `MainMenu` | **+ `MainWorld`** |
| enforced control-server token | `TESTVALUE123` | **64-char rotated** |
| `.utoc` / `.ucas` | 194,996 B / 125 MB | **268,036 B / 284 MB** |

No code changed ⇒ `BuildCookRun` compiled nothing and the archived exe kept its **compile** time
(G92's warning, biting from the other side).

⛔ **Every A44 hash reference in this project identifies only HALF the artifact.** *"Staged exe
`101AFEA4` = m25"* is still true and **no longer sufficient — two builds answer to that hash.**
⚠ **The reverse is the dangerous case:** a code-only hot-swap (G103) moves the **exe** and leaves the
**pak**. The halves move independently, so **a same-hash comparison is not a same-build comparison in
either direction.**

**How to apply — build identity = exe hash + pak identity.** The current measurement build:
`exe 101AFEA4` · `utoc 939B9C9B` (268,036 B, 2026-08-19 17:00:27, 4 maps) · `ucas 8A602D4D` ·
`pak 7CAE22DD`. Baseline exes live at **`D:\IntrusiveAnomalies\_binary_baselines\`** (outside
`Builds\`, hash-verified, README maps each hash to what cites it) — note they are **exe-only**, so a
baseline exe alone does not reconstruct a build either.

Also measured that cook: **the archive step wiped NOTHING** (leg dirs 56→56, baselines 4→4, `Saved\`
23→23) — but that was one flag set (no `-clean`, archiving into an existing tree), the 2026-08-16 wipe
is **not** retracted, and **the evacuate-first precaution stays**. Gotcha **G121** in `docs/gotchas.md`;
see [[path-a-blocked-mainworld-not-cooked]] for what the cook was for.
