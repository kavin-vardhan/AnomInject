---
name: s4-certified-m25
description: "S4 complete and tagged m25 (2026-08-19) - the backbuffer is demoted to the UI-on option, SVE is the default grab point, delivered frames no longer contain game UI"
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-08-19T07:25:57.962Z
---

**S4 COMPLETE, TAGGED `m25`** (annotated, carrying its own scope; `ebf1f16` → `d8482a2`, pushed).
Plugin HEAD `caaac09` (close-out docs). `CaptureBench` `bb79012`, local-only. **Bank ~90 dirs.**
Staged exe `101AFEA4` = m25; baselines beside it: `.s4-2-baseline` 259BF64F · `.s4-0-baseline`
834BB30A · `.m24-baseline` 3BA854FB · `.m23-baseline` 85A39CFB.

**THE DEFAULT GRAB POINT IS NOW SVE / SCENE-COLOUR — DELIVERED FRAMES CONTAIN NO GAME UI.**
`IAI.Capture.SVE 0` is the named bisect switch (backbuffer = the UI-on option, unchanged, deliberately
NOT renamed). The shipped default needs **no ini key** — that is what makes it immune to G88.
`run_summary.capture_path` is now emitted on **both** paths (`"sve"`/`"backbuffer"`); `key_ring_*` stay
SVE-only. **C1's leak-check invariant is FORMALLY RETIRED** (a ruling, not a side effect).

**Certified:** rect equivalence across **ten legs** (windowed / fullscreen 1920×1080 / 5:4 1280×1024 /
odd 1281×721 / non-multiple 1001×721 / SP50 / SP170 / verified DPI-aware at 150 %), all four rect
sources agreeing on every leg · 42/42 events ALIGNED and decidable on the six judgeable legs · UI
exclusion+presence by positive pixel check (max(R−B) 1.00000 vs 0.00392, control band 0.004) on an
identical-bbox pair · delivery orthogonality re-asserted (extras 0, 127 invariants positive).

**Limits that travel with the tag:** alignment certified at **1280×720 and 1281×721 ONLY** (B1's
`CALIB_BBOX` is frozen in PIXELS → unverifiable on BOTH grab points, an INHERITED gap) · A52 VideoFps 30
pinned · UI exclusion verified against **Canvas AHUD**, Slate/UMG **REASONED not measured** · one box,
one display, CB_GateLevel; true letterbox and multi-monitor UNMEASURED. **S4 does not close `P1`.**

**⚠ `m25` carries a CORRECTION to `m24`** (tag objects cannot be amended): of the five `key_ring_*`
counters only `missed`/`corrupted` are invariant; `published`/`consumed`/`wrapped` are **RUN-UNIQUE**.
m24's verdicts undisturbed. **Second instance of a certified claim resting on prose not measurement**
(G106 was first) — the control pair is authoritative over the prose.

**New gotchas:** G111 (a working-agreement in a shared doc is executed by any agent that reads it — Code
emits no 🔴 and routes decisions to chat-Claude, never the owner) · G112 (untracked config needs a
DETECTOR not a memory; the `TESTVALUE123` token was rotated 2026-08-19, value not in git) · G113 (the
Bash tool exits 1 on every call here while producing correct output — use PowerShell) · G114 (**a
packaged UE game is DPI-UNAWARE** — a display-scale change never reaches it; force `~ HIGHDPIAWARE` and
VERIFY with `GetProcessDpiAwareness`, or the null is an artifact) · G115 (**a shell round-trip
re-encodes the whole file — the tell is the DIFFSTAT, not the text**; standing pre-commit habit: read
`git diff --stat` before every commit).

**FORWARD QUEUE:** `H4` → `P5`/`P7` blend-ladder → `P1`'s `H1` lever (**H1 is P1's only named lead and
has no lever; design chat-side first, never same-turn as its first measurement**). Later/unordered:
`B2` · **B1-NDC** (unblocks alignment at any resolution) · `P6` implementation · A17/A19 · resolution /
JPEG / defaults profile. See [[s3-certified-m24]].
