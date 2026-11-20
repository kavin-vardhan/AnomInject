# 094-43a — FZ9: did any recorded 5.1 scanner reading show a custom-depth reader? (Claude Code, read-only, 2026-11-03)

**Answer: yes, but only on Lyra, our home fixture. Never on StackOBot. No 5.1 scanner reading from either office host exists at home.** These are observations, not rulings.

**The 5.1 scanner** is the m41 `HOST-PP CUSTOM-DEPTH READERS` preflight (`AnomalyCaptureSubsystem.cpp:2417-2549` at `53bf725`). It runs once at `StartRun`, and only when the census is effective (`:3427-3432`). So every run that printed it also had census custom-depth tagging on its captured frames, including frames labelled clean. It counts blendable post-process materials whose shader map reports `UsesSceneTexture(PPI_CustomDepth|PPI_CustomStencil)`. The 5.1 scanner has **no `unknown` category** (that exists only in the 4.25 port). It cannot see readers outside the material system (a scene-view extension, a custom pass, Niagara, UMG, decals).

**Method:** I walked the home roots on D: and E: for `run_summary.json` and logs. 196,072 files were seen; the bench bank on E: is now empty. I found **731 printed scanner lines** (674 in engine logs, 57 in run logs) and **93 `run_summary.json` files** carrying the key. I also searched every local branch's docs, `_reviews`, `_mailbox` and the typed reads in `_bates_reads`. Scripts and raw output: `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-43a\fz9\`.

## Per host

| Host / level | Dates | Scanner ran? | Result (scanned volumes/camera blends/entries/materials) | Tagging on? |
|---|---|---|---|---|
| **Lyra** `L_ShooterGym` | 09-04 → 09-25 | yes, 53 engine logs | **1 reader every time**: `MaterialInstanceDynamic_N(depth+stencil)`, 0/1/1/1. Journals 072/072-02 identify it as `MPP_Outline`, via the camera blend. | yes (census) |
| **Lyra** `L_Convolution_Blockout` (LG-9 legs) | 09-04 → 09-06 | yes, 3 | **1 reader**, same material, 1/1/1/1 | yes |
| Lyra running CaptureBench levels (`CB_GateLevel`, `CB_TexCorruptLevel`, PIE) | 10-15 → 10-16 | yes, 10 | 0, 0/1/0/0 (camera blend scanned, no blendables) | yes |
| StackOBot `CB_GateLevel` (packaged + PIE) | 09-03 → 10-15 | yes, 171 | 0, 0/1/0/0 | yes |
| StackOBot `CB_TexCorruptLevel` / `CB_LodFixture` | 09-27 → 10-16 | yes, 319 | 0, 0/1/0/0 | yes |
| StackOBot `MainWorld` | 09-03 → 09-29 | yes, 116 | 0, 1/1/0/0 (111) or 0/1/0/0 (5) | yes |
| **Concorde**: the client's M2 session (`session_20260901-165056`, bundle `2b6c93f`) | 09-01 | **no** (bundle predates m41) | none | **yes**: the client ini set `bMaskMeasureDefault=True`, so m26 tagged event targets on armed frames (journal 071 §1). The census did not exist yet (m36 merged 09-01; the bundle is from 08-23). |
| **Bates**: m36 Section B (census forced ON by console) and the m40 validation | 09-02 | **no** (pre-m41 builds) | none. The `reserved=` line, the host's own custom-depth list, was cut off in the photo. | yes for Section B; the m40 run is not recorded |
| Both office hosts: m52/m53/proxy PIE runs (`b5f15a3` and later) | 09-28 → 10-03 | the census is ON by default from m41, so it should have printed | **never transcribed**: no typed read mentions the line | yes (census default ON) |
| Any office or client capture on a post-m41 build (e.g. the 10-05 release) | — | — | nothing at home. Card Section E (Bates m41 read, which asks for this exact line) and checklist `C-G1b` have no recorded result. | yes |

## What could carry the leak
- **Lyra home sessions could carry it.**
  - A reader is present.
  - The census tags candidates on every saved frame.
  - `MPP_Outline`'s stencil test passes for any non-zero stencil (journal 072-02 §2), so every tag in 200..254 is inside it.
  - 074 Leg B measured "no detectable effect", but it classified frames by a census-arm proxy and carried no per-frame stencil read. It neither confirms nor rules out the leak.
  - These are bench evidence only; none was delivered.
- **StackOBot sessions cannot carry it through a blendable material:**
  - 0 readers on every recorded map, with the camera source scanned;
  - no post-process blendables at all;
  - StackOBot writes no host custom depth.
  - Non-material readers are outside the scanner's reach.
- **No office or client 5.1 dataset can be judged from home records.**
  - Tagging was active in all of them:
    - m26 since the m27 ini (blinking's visible in-window frames and missing_object's post-revert frames, both labelled clean);
    - the census on clean frames from m41;
    - m45 keeping custom depth on hidden targets, which is the "outline of a missing object" shape.
  - No host reader reading exists:
    - pre-m41 builds have no scanner;
    - post-m41 readings were never typed home.
  - Bates is recorded as writing host custom stencil on some actors. That is a writer, not proof of a reader, but it raises the risk.
  - The client's eight M2 findings mention no outline. That is an absence of complaint, not a reading.
