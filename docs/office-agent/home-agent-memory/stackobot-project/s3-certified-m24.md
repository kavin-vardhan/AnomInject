---
name: s3-certified-m24
description: "S3 SVE capture migration CERTIFIED and tagged m24 (2026-08-18) — ratio-independence discharged, delivery mode orthogonal, P1 narrowed to H1 only"
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-08-18T16:54:20.499Z
---

**2026-08-18 — `S3` IS CERTIFIED AND TAGGED `m24`** (`AnomalyInjector` `d429f3b`, tag pushed and
remote-confirmed; tree clean). Supersedes the "S3a certified / S3b not started" state.

**What certified:** ratio-independence across nominal (1.0000), client band ×2 (1.2548, 1.2397), deep
starvation (3.0308) and pacing-off (0.3327) — **33 counted events, 33 ALIGNED, 33/33 decidable**,
positive control decisive both directions on every leg. Plus **delivery-mode orthogonality** — three
pairs, zero extras, invariant core identical including all five `key_ring_*`.

**Scope limits that travel with the tag:** modal camera pose ONLY · `VideoFps` 30 pinned (A52) · Stage 3
is annotation-shape evidence only · the oracle's margins are not reproduced above 30 fps.

**🚨 `P1` NARROWED, NOT SOLVED.** The delivery-mode lead is **eliminated by measurement**. **`H1` is P1's
ONLY remaining named lead and H1 has no lever in existence — if H1 comes back clean, P1 has NO named
leads.**

**The load-bearing lesson of the arc (→ G106):** the A54 oracle existed only in prose, so **every prior
certification (I10, m23) had been graded by scripts that no longer existed**. Rebuilding it exposed two
under-specified points where the attractive reading was wrong, each of which left every *verdict* correct
while silently halving every *confidence*. **It was caught only by reproducing published NUMBERS rather
than published CONCLUSIONS.** Analysis instruments that grade a certified result are now committed
artifacts, in `CaptureBench/tools/` (local-only repo).

**Next:** the **I10 hand-chosen-window re-check** (paper only, banked data, no runs — I10's null is
**weakened, not overturned**), then **S4** = backbuffer demotion, which is a **client-visible change**
(C2), not a silent default flip. Depth stays parked and unnumbered.

Full record: `docs/sessions/2026-08-18-042-s3b-matrix-certified.md`. See [[anomaly-injector-state]].
