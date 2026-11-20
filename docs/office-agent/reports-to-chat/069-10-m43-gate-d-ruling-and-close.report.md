# REPORT for 069-10-m43-gate-d-ruling-and-close.md
brief: 069-10-m43-gate-d-ruling-and-close.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: medium
started: 2026-09-03 02:55:49 | finished: 2026-09-03 03:08:05 | duration: 12 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-10-m43-gate-d-ruling-and-close.live.txt

---- FINAL MESSAGE (Claude Code) ----
`m43` is closed and on master.

---

# SESSION 069, BRIEF 10 — `m43` SHIPPED

## 1. The two outstanding gates

**E(vii) letterboxed fixture — PASS.** ⚠ `run_leg.ps1`'s `-LetterboxedFixture` only *relaxes B1*; it never issues the lever, so the first attempt read `rect=(0,0)-(1280,720) — ORIGIN IS ZERO` and the harness's own self-policing assertion correctly refused it three times. Supplied explicitly (`Set PlayerCameraManager bDefaultConstrainAspectRatio true, … DefaultAspectRatio 2.39`):

- picture **1280×536**, rect origin `(0,92)` — constraint proven from the artifact
- **mask PNG 1280×536 == `Actual_Frames` 1280×536** ✅
- **MASK-TIE 29 lines, 0 MISMATCH** · coverage **29 + 61 = 90**, unavailable **0**, 90 files

**E(x) graceful shutdown — PASS.** ⚠ Two attempts were **vacuous and were thrown out, not reported as passes**: `-testexit` matched the engine's *own echoed command line* and quit before capturing anything — exit 0, no asserts, **no session**. The leg that counted ran to completion, then closed the window:

- **exit code 0** · `Object subsystem successfully closed.` **×1** · Fatal/Assertion/Ensure **0**
- 30 captured, `measured=5 hiddenBlank=25 unavailable=0`, **30/30 masks flushed**
- **folder deletable = YES** (no leaked handle)

## 2. SHAs, exe, branch

| | |
|---|---|
| `master` HEAD == `origin/master` | **`8bd32a0`** ✅ verified equal |
| feat commit (one) | **`b9d9ea7`** — `feat(capture): m43 - target ID mask; one mask render serves all pending arms` |
| history | `2691f8c` predictions → `e6f5270` ADDENDUM 2 → `48d8ed9` ADDENDUM 3 → `b9d9ea7` feat → `8bd32a0` docs |
| squash integrity | tree vs pre-squash tip differs only by the ruled tag-flip counter (12 ins / 4 del) |
| **branch deleted** | ✅ **yes, origin and local.** Origin refs: `master`, `feature/mask-gpu-reduce`, `feature/selection-census`. **No reachable ref to the dead name, either side.** |
| exe | **`AD543F42`** (241,169,920 B) validated; **`5C073AC9` = `m43`'s A-side**; container UNCHANGED, no cook |
| tags | none |

⚠ **Caught and corrected mid-commit:** `git add docs/` swept in the owner's five untracked `CHAT-HANDOFF-*.md`. Unstaged before committing — they remain untracked, as the standing rule requires.

## 3. The three echo branches, verbatim

**ON**
```
=== Capture(m43): TARGET MASK ON FOR THIS RUN - requested on, from COMPILED DEFAULT (on), output dir
'M43G_LBOX2_try1/session_20260903-030010/target_mask' === READ THIS LINE, NOT THE INI. One 8-bit grayscale
PNG per captured frame, numbered by SESSION INDEX; non-zero pixel values are the stencil tags of the
ANOMALY TARGETS visible in that frame and 0 is background. mask_map.json maps value+event to target and
anomaly type. A BLANK png means MEASURED AND NOTHING VISIBLE (a hidden blinking target); mask_file:null in
labels.jsonl means NOT MEASURED - they are different facts. It reuses the m26 pass and does NOT change the
m26 measurement, the veto, or annotation.json. Delivery mode does NOT suppress it.
```

**OFF**
```
=== Capture(m43): TARGET MASK OFF FOR THIS RUN (requested off) - requested off, from
IAI.Capture.TargetMask (console), output dir 'M43F_TMOFF_CENON_try1/session_20260903-023958/target_mask'
=== READ THIS LINE, NOT THE INI. […same body…]
```

**REFUSED**
```
=== Capture(m43): TARGET MASK OFF FOR THIS RUN (REFUSED: IAI.Capture.OutputHeight is non-zero. The mask is
view-rect sized while m28 RESAMPLES the written frame, so the two would disagree in size, and a LABEL MASK
MUST NEVER BE FILTERED - bilinear would invent stencil values that were never assigned to anything.
Nearest-neighbour mask resampling is a named follow-up, NOT built. Set the output height to 0 to get
masks.) - requested on, from COMPILED DEFAULT (on), output dir '…/target_mask' === […same body…]
```

## 4. The tag-flip count

**`tagFlips = 0` on every leg** — baseline, letterboxed, and the shutdown leg. The target mask never tagged anything itself, because `m26` already had the live target tagged whenever it armed. **The churn is zero on this bench; its effect on pixels is UNMEASURED, not shown harmless.**

🚨 **This refutes a mechanism I asserted last turn.** I attributed gate D's `tagOvertaken` 0→1 to *"the target mask's tag/restore cycle opening windows"*. With `tagFlips = 0` that cannot be the cause — the perturbation comes from the extra arms changing census batch timing. **Recorded as refuted in the ledger and journal, not quietly dropped.**

## 5. NEEDS-DECISION

**None.** `m43` is on master, untagged, with the office batch now `m31 → … → m40 → m41 → m43`. Two things are queued and neither needs a ruling from me: **`m42`** (persist tags, measurement-first, with its own motivation already corrected in the ledger) and **`m39`** (honest bbox). Bates reads the mask via card **E-6**.

📌 Worth carrying forward: **two of the four `m43` stops found defects in already-shipped code, not in the new feature** — the `m26` arm starvation (a veto input coupled to census cadence, `fc 2→4` on one event, latent) and the vacuous-pass trap in my own shutdown harness.

**STOP.**
api-equivalent cost (not billed on Max): $22.36 | turns: 32 | running total across all briefs: $274.64
