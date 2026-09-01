# m31-S1 BRANCH TABLE — the instrumented Concorde run

**PRE-REGISTERED 2026-08-21, committed WITH the S1 instrumentation, BEFORE any instrumented
run exists.** Restate this table verbatim before any result is read.

## Context, one paragraph

First run of the plugin on a second host (Concorde, UE 5.1 source-built, packaged exe name deliberately withheld (codename-only invariant; visible on the office box itself),
m27 build): the SVE path — the shipping default since m25 — produced ZERO frames while the
backbuffer path (`IAI.Capture.SVE 0`) wrote PNGs normally on the same build and machine. The
office-side diagnosis (relayed, treated as evidence to verify): zero `Capture(sve): keyed frame
id=... submitted` lines, in-flight list empty at every drain (`samples=0 notReadyPolls=0`),
`pendingAtDrainEntry=120 ... pendingAfter=120`, ring healthy (`published=332 consumed=332
missed=0 corrupted=0`), and the only silent exit on the chain is the `!Entry.bWanted` return —
so `bWanted` false on all keys is INFERRED from the 0/120 hit rate, not OBSERVED. m31-S1 exists
to close exactly that gap. Source verification on the canonical box confirms the inference's
premises (see journal 049 Task 1): the `!Entry.bWanted` return is the only silent exit between a
successful `LookupKey` and submission, and publish-time is the only writer of `bWanted`.

## What S1 measures

- `wantedPublished` — publishes with `bWanted == true`, counted beside the existing ring
  counters, reported in the run-end key-ring block and as `run_summary.wanted_published`.
- A bounded per-publish trace (first 64 publishes, compiled constant): family frame number,
  publish-time `GFrameCounter`, `bWanted`, the capturer's last-marked frame, and the signed
  offset (publish `GFrameCounter` minus last-marked). One line per publish, so repeated
  `GFrameCounter` values against differing family numbers — the multiple-view-families-per-frame
  candidate — are readable directly from the lines rather than averaged away.
- An unconditional run-end summary: `wantedPublished=X of Y publishes` plus min/max/mode of the
  offset across the traced window.

## THE BRANCH TABLE

```
R-1  wantedPublished ~= 0, offsets consistently non-zero
     -> the handshake miss is OBSERVED, not inferred. THE MEASURED OFFSET BECOMES THE
        CALIBRATION for the fix. Fix design goes to chat WITH THE NUMBER.
R-2  wantedPublished ~= the wanted count (~120)
     -> the office inference was WRONG in the useful way: the loss is between publish
        and lookup, and the ring becomes the suspect. STOP. Report. No fix aimed
        anywhere.
R-3  partial, mixed, or unstable offsets
     -> report the distribution VERBATIM. No branch is forced. Chat rules.
ALL BRANCHES: PIE licenses mechanism only (G76). THE FIX, when built, validates PACKAGED
with a same-seed before/after against the broken run — the m27 count-gate shape.
```

## PRE-REGISTERED, so the fix debate is not had twice

The three candidate fixes are known and NONE is authorised:

1. **FIFO pairing, mirroring the working backbuffer path** — costs exact-frame identity, and
   `PendingSnapshots` in `AnomalyCaptureSubsystem.cpp` is keyed strictly by `GFrameCounter`
   (see the pairing fact below).
2. **A tolerance window on the render side** — NEEDS A NUMBER NOTHING CURRENTLY CALIBRATES,
   which is precisely what R-1 would supply and why it cannot be chosen first.
3. **Instrument first** — that is m31-S1 and it is what is being built.

Chat rules the fix once the number exists.

## THE PAIRING FACT — established read-only, decides option 1's viability later, not ruled on now

At the S1 commit's line numbering (`AnomalyCaptureSubsystem.cpp`): `PendingSnapshots` is a
`TMap<uint64, FCaptureSnapshot>` (:102), added at :1448 keyed by `Snap.FrameCounter`, which is
`GFrameCounter` at arm time (:1436). Pairing is an exact `TMap::Find` on the completed frame's
`RequestId` (:1251) — **no tolerance window exists**. If a captured frame slipped by exactly one
engine frame, two sub-cases follow from the lines:

- **The off-by-one key is NOT pending** (burst edge, non-adjacent arming): `Find` fails and the
  frame is dropped with a VERBOSE-level log (:1252-1257) — invisible at default verbosity — and
  the orphaned snapshot surfaces only in the run-end "did not resolve" warning (:1353-1357).
- **The off-by-one key IS pending** (burst interior — consecutive armed frames are outstanding
  simultaneously, guaranteed by the ≥1-frame readback latency): `Find` SUCCEEDS on the ADJACENT
  frame's snapshot — a SILENT MISPAIR. The pixels are written under the neighbour's session
  index and label (:1265-1286), the neighbour's snapshot is removed (:1288), and the true owner
  of that key later drops via the first sub-case.

So exact-key pairing does not tolerate a one-frame slip: it either quasi-silently drops or
silently mispairs, depending on whether the neighbour key is pending. This fact is recorded for
the option-1 debate and is NOT a ruling on it.

## Tokens

The per-publish trace lines carry `SVE-WANT-TRACE`; the run-end summary carries
`SVE-WANT-SUMMARY`. Both verified unique repo-wide before this commit. Per the veto-object
discipline the summary log line does not repeat the trace token's literal, so a grep for the
trace token returns exactly the per-publish lines.
