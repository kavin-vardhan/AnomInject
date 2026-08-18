# 2026-08-18 — 038 — S3a-2's gate failure, diagnosed: an `else` that changed owner

**Diagnosis turn. NO FIX. No line moved, no re-run for a green.** The failing code stays on
`s3a-2-GATE-FAILED-do-not-merge` (`087f4d9`); `master` is untouched at the certified S3a-1 state.

---

## 1. The mechanism — confirmed from source, both leads eliminated

`FinishRun` on `master` is a single **if/else**:

```cpp
if (bRunBegun)
{
    ... drain, pacing, annotation, run_summary ...
    if (bLogLine) { UE_LOG(... "=== Capture run FINISHED: ..." ...); }
}
else
{
    IFileManager::Get().DeleteDirectory(*RunDir, false, true);
    if (bLogLine) { UE_LOG(... "=== Capture run CANCELLED before focus: ..." ...); }
}
```

The two log lines are **mutually exclusive within one call**. The failing leg printed **both**.

S3a-2 appended a block immediately after the closing brace of the `if (bRunBegun)` body — and the
next token in the file was `else`. On the branch (`AnomalyCaptureSubsystem.cpp:1467-1482`):

```cpp
    }                                                    // <- closes if (bRunBegun)

    if (Async.IsValid() && Async->SveCapturer.IsValid())  // <- inserted
    {
        Async->SveCapturer->SetActive(false);
    }
    else                                                 // <- WAS the else of if (bRunBegun)
    {
        IFileManager::Get().DeleteDirectory(*RunDir, false, true);
        ... "CANCELLED before focus" ...
    }
```

**The `else` silently changed owner.** Consequences, all three observed:

1. `if (bRunBegun)` lost its else entirely — the cancel path no longer guards cancelled runs.
2. The **delete** now fires whenever the SVE capturer is absent — which, **with the switch OFF, is
   always**. Every switch-OFF run writes a complete session and then deletes it.
3. It happens **inside one `FinishRun` call**, which is why `FINISHED` and `CANCELLED` are 20 ms
   apart. **The 20 ms is the recursive delete of 90 PNGs.** The timeline closes exactly.

**It compiles clean.** `if/else` is valid C++ either way; no warning, no type error. Nothing but
reading the surrounding text catches it.

### Both of my own leads are ELIMINATED, not merely deprioritised

- *"The `SetActive` teardown appended after the `bRunBegun` block"* — implicated, but **not** as a
  teardown running too early. Its `SetActive(false)` call is a harmless no-op with the switch off.
  It is guilty only as the **anchor that captured the `else`**.
- *"The log line inserted between `bRunBegun = false` and the focus-gate branch in `StartRun`"* —
  **entirely innocent**. It changes no control flow.

`FinishRun` was **not** called twice. That hypothesis is dead too.

---

## 2. BLAST RADIUS — `m23` and every shipped client build are NOT affected

**Answered from source, explicitly, because the delete path destroys a session directory by design.**

On `master`/m23 the if/else is intact, so within one `FinishRun` call the write and the delete are
mutually exclusive. Reaching the delete after a successful write would require a **second**
`FinishRun` call with `bRunBegun == false` and `RunDir` still set. Every caller is guarded:

| caller | guard | after a finish |
|---|---|---|
| `StopRun()` (`:731-740`) — the control server, `Deinitialize`, console Stop | `if (!bRunning) return;` | `bRunning == false` ⇒ **unreachable** |
| `Tick` `:342` (frame cap) | `Phase != Idle && Phase != DrainTail` | `Phase == Idle` ⇒ **unreachable** |
| `Tick` `:383` (`case PostGap`) | phase switch | no `case Idle`; falls to `default: break` ⇒ **unreachable** |
| `Tick` `:396` (`case DrainTail`) | phase switch | same ⇒ **unreachable** |

`FinishRun` ends with `bRunBegun = false; bRunning = false; Phase = ECapturePhase::Idle;`
(`:1356-1358`), which is what closes all four.

⇒ **`FinishRun` executes at most once per run on m23. A successful run cannot take the delete
branch. This is NOT a client-facing data-loss defect.** It is confined to the S3a-2 branch and was
introduced by this session's edit.

**One latent observation, reported without overstating it:** `RunDir` is **not** cleared at the end
of `FinishRun` (`:1356-1364` clears the targeting state but not the path). Nothing reaches it today —
all four callers are guarded — but it means the delete branch is one unguarded future call away from
destroying a *previous* session. A hardening candidate (clear `RunDir`, or guard the delete on
`RunDir` belonging to a run that wrote nothing), **not** a defect and **not** in scope now.

---

## 3. What this earns

### A62 — for any gate whose subject is written output, THE ARTIFACT ON DISK IS THE GATE

Both legs' logs read **identically and perfectly**: same `STARTED` line, same
`FINISHED: 90 frame(s) (positive=59) | 7 burst(s), 0 zero-match`. A log-gated check calls the failing
leg **green**. The defect is visible only by looking at the directory. This is **m19's lesson
recurring in a new place** — there it was "a frame counter incremented" on a build emitting black
frames; here it is "FINISHED" on a build that deleted the session.

**A log line saying `FINISHED` is not evidence that a file exists.**

### C1 amended — the error is chat-side, recorded as such

C1 required byte-identity of `annotation.json`, `labels.jsonl` **and** `run_summary.json` with the
switch OFF, to remove an exclusion from the gate. That rule is **unsatisfiable by construction**:
`session_id` is a timestamp and appears in `annotation.json` and `video.path`; `run_summary` carries
`end_frame` (an absolute `GFrameCounter`) and the wall-derived `speed_ratio` /
`sustained_wall_fps`. Two runs of the *same* binary cannot satisfy it.

Same class as the m23 micro-plan contradiction and the S3a-1 cvar contradiction: a brief requiring
two things that cannot both hold.

**G-S3a-1 (AMENDED) — strictly stronger than a hand-waved allowance:**

1. **CONTROL PAIR** — two runs of the **same m23 binary**, same seed, same config. Their difference
   set *is* the **run-unique field set**, established **empirically** and recorded field by field.
2. **SUBSET TEST** — the m23-vs-S3a-OFF difference set must be a **subset** of step 1's. Any field
   that differs but did **not** differ in the control pair **fails**. No judgement call.
3. **FRAME IDENTITY** — decoded marker ↔ `frame_index` series, count, cadence.

The banked `S3A2_BASE` leg is one half of the control pair **if its `run.json` config matches
exactly** — verify, do not assume; produce a second m23 leg either way.

### Gotchas G101, G102, G103

See `docs/gotchas.md`. In short: `outDir` is CWD-relative; appending after a closing brace can steal
an `else`; and staging is an exe hot-swap, so **G92's archive-wipe path is not involved**.

---

## 4. State

- `master` — **`44e5f5b` + this docs commit.** S3a-1 only. Certified, clean.
- `s3a-2-GATE-FAILED-do-not-merge` — **`087f4d9`**, pushed. **Do not merge.**
- Staged package — **restored to the m23 binary and re-scanned** (`IAI.Capture.SVE` count 0).
  Back to the exact state every banked leg was produced with.
- `CaptureBench` — `8dad64e`, probe untouched.
- Banked: `S3A2_BASE\session_20260818-110348` (a clean m23 30 fps gate leg — 8 events, gapped
  `[4,5,9,10]`, `manifested` 8/8, ratio 1.0000004) and `S3A2_OFF_FAILED_EVIDENCE\` (both run logs).
- **Next: the fix, as its own turn.** Diagnosis and fix do not share a turn.
