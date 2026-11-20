# REPORT for 080-02-m52-stuck-low-mip-implement-and-gate.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 080-02-m52-stuck-low-mip-implement-and-gate.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-20 19:22:42 | finished: 2026-09-20 19:58:09 | duration: 35 min | exit code: 0
context at last turn: 782616 tokens (78% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\080-02-m52-stuck-low-mip-implement-and-gate.live.txt

---- FINAL MESSAGE (Claude Code) ----
Everything committed and pushed; checkout restored to `m51`.

---

# 080-02 — m52 `stuck_low_mip` implemented and gated — REPORT

**Branch `feat/m52-stuck-mip` = `origin/…` = `ced1d80`** (3 new commits). `master` = `origin/master` = `3ff88db` **unchanged**; `m51` = `53bf725` **unchanged**, checkout restored there, tracked-clean; CaptureBench `472a409` untouched; 0 StackOBot processes. ⛔ No merge, no tag, no cook, no default flipped, no threshold tuned.

## Binary
| | |
|---|---|
| **built == staged == archived** | **`A8742A4A`**, 241,468,416 B → `_binary_baselines\StackOBot.exe.m52-stuckmip-A8742A4A` |
| predecessor | **`D50DDE78`** (m51 F1 candidate) **hash-verified at its archive before** the staged copy was overwritten (A62) |
| container quintet, before **and** after | `67EA1FE0` / `2CEFB8F4` / `E03C6610` + `A16A18A8` / `C70ECDAA` — **BYTE-UNCHANGED**, code-only hot-swap, no cook (G103) |
| A44, both encodings | 11 new symbols present UTF-16 / absent ASCII; 3 pre-existing controls present (**sound**); 2 invented symbols absent (**discriminates**) |

## Gates — exact lines
- **G0 build** ✅ `StackOBotEditor Win64 Development` **exit 0, zero warnings** + `StackOBot Win64 Development` **exit 0, zero warnings**. The modular target is the one that would catch §7's devirtualisation hazard.
- **G1** ✅ `--- IAI.DumpCatalog (10) ---`; `stuck_low_mip | scope=object | usage='<substring> [mip_levels]' | args: mip_levels:int[-1.0..-]=-1`; `Default pool: blinking, missing_texture, corrupted_texture, lod_popping` (4, without it); `PRECONDITION READ-BACK r.TextureStreaming=1 r.Streaming.UseAllMips=0`.
- **G2 can-fail** ✅ `held` **false 51/51**, `observable` **null 51/51**, `bench_no_hold` on every row, **all 7 events `injected=[] affected=[] manifested=false`**, `observable_frames 0`, `stuck_mip_frames_held 0`. *Deviation from the pre-declaration: `observable` is **null**, not `false` — no mask is armed on an unlabelled frame, so nothing was measured. The null is the more honest value.*
- **G3 hold** ⚠ **PARTIAL.** Works: resident **11/12 → 7**, `top_resident_px` **2048/1024 → 64**, `observable true`, `target_pixels` 32,430–38,184, `predicted_max_allowed=7` on every hold, zero held→unheld mid-window. **But only 4 of 7 fires held at all, 22 of 51 window frames.**
- **G4 restore** ⚠ 7/7 `restored=N left-to-game=0 unresolved=0`; **2 of 5 exit paths** — cancel-before-focus, target-destroyed, level-change **UNRUN**.
- **G5 onset** ✅ *first labelled frame has `held:true` on every manifested event*, **both tick orders**. Latency measured **1, 2, 3, 4** frames on the four that landed.
- **G6 hitch** ✅ m52 `1.0000017 / 29.99995 fps` vs blinking control `1.0020464 / 29.93873` — m52 **0.068 ms/frame faster**. ⚠ measures **pacing, not max frame time**; that delta is *below the resolution of this instrument* (G169), not "no cost".
- **G7 yield** 📊 below.
- **G8 Lyra** ⛔ **UNRUN** — so the virtual-texture NOT_APPLICABLE path has **never fired** (`stuck_mip_refused_virtual` read 0 everywhere).
- **G9 schema** ✅ row keys **13→13** (+0/−0); anomaly keys **12→23**, exactly the 11 `stuck_mip.*` keys and **only inside a `stuck_low_mip` entry**; `run_summary` **+8 `stuck_mip_*`, −0** vs a pre-m52 session; `annotation.json` **4→4, P6 does not move**; `label_schema` **2**.
- **G10 both orders** ✅ native and synth **identical**: fires 7, manifested 4, held 22, onsets `[None,4,2,1,None,3,None]`, same `speed_ratio`.
- **G11 verifier** ✅ `NO FAILURE FOUND`, exit 0, **NO-TRACE 0**. 🚨 **but it printed *"NO-TRACE unavailable for class `stuck_low_mip`"* — the only failure verdict is unreachable for this class, so the zero is weaker than it looks.**

## G7 yield — MainWorld, 600 frames, pool = `stuck_low_mip` only
```
fires_applied 32   manifested 6 (18.75 %)   non_manifested 26
frames_held   29   of 256 window frames     observable_frames 17
refused_shared 177 | not_streamable 133 | no_eligible 17 | virtual 0 | imperceptible 0
```
**18.75 % is below chat's 25 % steer ⇒ recommendation: do NOT flip default-on.** It is in `GAutoPool`, not in `GAutoPoolDefaultEnabled`.

## Three findings — reported, not fixed in the same turn
1. **A re-fire on an already-held target refuses.** At settle K=8: **zero held frames all session** — which *refutes* "a longer settle fixes onset". Log: `already at or below the requested resident mip count (resident 7, target 7, floor 7)` ×24, `HELD NONE` ×4. The texture hadn't returned to its pre-anomaly count before the next fire. **First event works, later events on the same target can silently produce nothing** — no false positive, yield collapses. ⛔ Mechanism **not established, not chased** (G120); one candidate *named not claimed*: the revert's `StreamIn` belt is guarded by `!HasPendingInitOrStreaming()` and is silently skipped when a stream op is in flight, with no log line.
2. **Onset latent 1–4 frames**, sometimes > the 8-frame window; 3 of 7 fires never held → `manifested:false`, empty frame lists. That's the m23 F-LABEL guard working.
3. **Mine:** `stuck_mip.resident_mips`/`baseline_mips` describe the **primary** texture while `held` is **any-of-armed**, so `armed=6 heldN=1` prints `11/11 held=true` and reads as self-contradictory.

New gotchas **G265** (`UTexture::LODBias` is a cooked-platform no-op — editor-green, client-inert) and **G266** (the streamer *cancels* a foreign stream request; no ordering of `UnlinkStreaming` works).

Chat rules on F1, F3's reporting shape, and whether 18.75 % justifies raising `MaxCoAffected` above 0 — which admits unlabelled blurry objects and is a dataset decision, not a tuning one.

VERDICT: NEEDS-DECISION
api-equivalent cost (not billed on Max): $41.45 | turns: 107 | running total across all briefs: $1,783.49
