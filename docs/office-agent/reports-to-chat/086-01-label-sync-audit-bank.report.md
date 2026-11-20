# REPORT for 086-01-label-sync-audit-bank.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 086-01-label-sync-audit-bank.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 20:10:31 | finished: 2026-09-28 21:44:26 | duration: 94 min | exit code: 0
context at last turn: 750849 tokens (75% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\086-01-label-sync-audit-bank.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

The audit is done, and every delivered anomaly is classified from banked pixels. Nothing was launched, built or changed in source. Chat needs to rule on four things before part B and the release gate.

**Plain summary for the owner**
- **In sync, 0 frames off on every edge we could measure:** blinking, missing_object, missing_texture and corrupted_texture. This holds at the "most of the effect is showing" threshold, in both tick orders, with anti-aliasing on and off.
- **One small leftover:** under the game's anti-aliasing, the first frame after a hidden object comes back still carries 12–16% of the effect on 4 of 592 blinking edges. The label timing is right; this is the picture catching up.
- **Out of sync:** stuck_low_mip on the current master build. The blur shows up 1–5 frames before the label and lasts past it. That matches the 084-01 diagnosis and your office reading. The fix is built but not yet bench-proven (084-03).
- **Not measured yet:** lod_popping and camera_clipping (no usable bank evidence), and the two m53 anomalies (their shipped modes aren't built yet). Most of the proven evidence is on the synthetic test level; realistic-content legs are in part B.

**The table** (offset = visible frame − labelled frame; positive means the picture lags the label; majority threshold unless noted)

| anomaly | window driver (source) | circular-gate history? | bank evidence (judged) | start offsets | end offsets | wrong-object | class |
|---|---|---|---|---|---|---|---|
| blinking | object hidden flag (`AnomalyHiddenClass.cpp:174`), toggled in the anomaly's tick (`Anomaly_Blinking.cpp:74-92`), read at end of world tick (`AnomalyCaptureSubsystem.cpp:885-892`) | no; past gates were pixel-based (m40, m44 onset, m49 G-EDGE) | 307 events, 106 sessions; synthetic level, MainWorld, Lyra | 680 × 0 | 597 × 0; at 10%: 588 × 0, **4 × +1** | 0 (2 candidates were the object's own shadow, checked by eye) | **SYNC-PROVEN** at 50%; +1 residual at 10% |
| missing_object | same flag; hide in `Apply` (`Anomaly_MissingObject.cpp:36`) | no; m49 G-EDGE was pixel-based | 36 events, 15 sessions; synthetic level only | 36 × 0 | 30 × 0 (6 censored) | 0 | **SYNC-PROVEN** (synthetic content only) |
| missing_texture | labelled for the whole fire; material swap (`Anomaly_MissingTexture.cpp:123/205/216`) | no; m44 and m49 were pixel-based | 25 events, 14 sessions; synthetic level, Lyra; AA on only | 25 × 0 | 14 × 0 (11 censored) | 0 | **SYNC-PROVEN, thin** |
| corrupted_texture | same shape (`Anomaly_CorruptedTexture.cpp:123`) | no | 130 events, 49 sessions; synthetic level, Lyra | 130 × 0 | 103 × 0 (27 censored) | 0 | **SYNC-PROVEN** (your office eyeball agrees) |
| stuck_low_mip | resident mips read from the game-thread mirror (`Anomaly_StuckLowMip.cpp:892`) | **yes**: onset gate was circular, offset gate couldn't see pixels (084-01) | 34 events, 13 sessions; MainWorld, Lyra | −1 × 10, −2 × 2, −3 × 4, −5 × 2, −20 × 1, 0 × 2 | +4 × 2; the rest run past the window (the frame after the label still shows 95% of the blur) | none tied to the label (the bank can't show your glove/wall case) | **DESYNC** |
| lod_popping | popped-phase flag flipped every 8 ticks with a forced LOD (`Anomaly_LodPopping.cpp:235-253`) | **yes**: m30 counted toggles from the anomaly's own counter | 36 events, **0 judgeable**: pops too small to measure, or drowned by MainWorld scene motion | — | — | — | **UNMEASURED** |
| camera_clipping | collision sphere of near-clip radius around the camera, player pawn ignored (`AnomalyViewport.cpp:857-885`) | **yes**: m30 checked the label against its own overlap test | **none** in the current label format | — | — | — | **UNMEASURED** |
| uv / normal corruption | labelled for the whole fire; render-target draw plus material swap in the fire tick (m53 branch `d117a9d`) | the onset pixel gate is planned but not run; the offset has never been gated from pixels | test fixtures only (non-shipped modes): 188 / 18 events | 188 / 18 × 0 | 155 / 15 × 0 | 0 | **UNMEASURED** for the shipped modes |

How the checker was proven to fail on bad labels:
- **Built-in known answers in the bank:** the test twin that turns magenta 3 frames after its label starts read **+3 on 12 of 12**. The do-nothing twin produced no signal on any of its 186 events.
- **Deliberately broken labels:** labels shifted or trimmed by one frame at either edge **fail at both thresholds, 98 of 98 checks**, and the correct labels pass.
- **It caught a flaw in my own first rule.** I had excused edges sitting next to the capture's settle gap, and that hid a real one-frame desync. I fixed the rule before reading any verdict.

The m55 change evidence agrees independently: the anomaly appears on the first labelled frame in 248 of 248 measured phases for blinking and missing_object.

**Mask finding that affects delivery:** a capture session with more than about 55 events runs out of mask IDs. In one 90-event leg, the last **35 events had labels but no target mask** (the stencil-tag limit already documented in m50).

**The out-of-sync items**
- **stuck_low_mip:** the label reads the resident-mip count on the game thread, which updates after the render thread has already changed the picture. At the end, the anomaly is dropped in the same tick the stream-in starts, so the label stops while the blur is still on screen. The fix (render-side residency per captured frame, event kept open until restore) is on `fix/m52-label-timing` `c71f826`, exe `B725678B`. 084-03 is the proof; this checker should run on its legs too.
- **The anti-aliasing leftover (hide types):** with anti-aliasing off it never exceeds 7.3%, so temporal anti-aliasing is the likely main cause. A slower lighting catch-up after the object is restored may add to it; neither is proven.
  - Suggested fix: an extra per-frame key marking the first frame after the object returns, so clients can drop it.
  - Rejected: extending the label one frame, because that would call a plainly visible object "missing".
- **camera_clipping (a structural risk, not a measurement):** the label can disagree with the picture in three ways. It counts geometry behind the camera, it uses collision shapes rather than what's drawn, and it ignores the player's own mesh. Suggested fix: test only the slice of the view between the normal and pushed near planes, including the pawn, and prove it with a matched no-clipping run.
- **lod_popping (a candidate only):** LOD transitions that fade over a few frames could lag the label. Part B leg B-LOD is designed to check this.

**Part B plan (bench, not run)** — about **1 h 50 min** of bench time plus retries:

| leg set | what it closes | legs | time |
|---|---|---|---|
| B-M52 | stuck_low_mip fix | already scheduled as 084-03; add this checker | 0 extra |
| B-LOD | lod_popping | rock at close range, both tick orders, anti-aliasing on and off | ~10 min |
| B-CC | camera_clipping | clipping on vs normal near plane over a scheduled camera path | ~10 min, **plus a small bench-only lever to build by day** |
| B-M53 | m53 shipped modes | runs inside 085-03, plus 4 MainWorld legs with a no-apply twin each | ~10 min extra |
| B-REAL | realistic content for the 4 proven types | 10 MainWorld legs, plus anti-aliasing-off twins | ~25 min |
| B-LYRA | second host with TSR anti-aliasing, camera locked | 6 legs | ~35 min |
| analysis | | | ~20 min |

Your environment-pack project would add real art for every anomaly: dense foliage, detailed textures, textures shared across many meshes, authored LOD chains, and close geometry.

**Office check kit (spec only)**
- A single `label_sync_check.py` for plain office Python (standard library only; uses Pillow if it's there, with matching numbers). It only reads session folders and prints numbers per anomaly:
  - event counts: judged, camera-moved, warm-up, not measurable;
  - start and end offset counts at the 10% and 50% thresholds;
  - censored and unresolved edge counts;
  - wrong-object count (an upper bound);
  - missing and extra masks;
  - the leftover-effect size;
  - the m55 onset check.
- **You need to:** capture PNG frames with the target mask, labels and m55 evidence all on (all defaults), keep native resolution, and **keep hands off so the camera doesn't move**.
- It runs a self-test first and refuses to report if it passes a broken label.
- It can't judge moving-camera events, camera_clipping, targets without masks, very faint effects, frames the capture skips, or anything off-screen. Wrong-object counts need an eye check.
- Runtime: 30–90 min with plain Python, 2–3 min with Pillow.

**Commits pushed**
- `ca9c98f` on the new branch `docs/label-sync-audit`, cut from `master` `b5f15a3`; head matches origin. It holds the journal only. No tag, no merge.
- The docs worktree is kept at `E:\IA_BuildCache\_r86_docs`.
- The main checkout is still `m51` `53bf725`, and `master` equals origin at `b5f15a3`.

**NEEDS-DECISION**
1. **What counts as a visible frame for the release gate.** At majority (50%), the four proven types are 0 off everywhere; at 10%, blinking has +1 on 4 of 592 edges under anti-aliasing. I recommend ruling at 50%, reporting the 10% and strict counts alongside, and adding the "first frame back" key.
2. **The mask-ID limit** (masks stop after about 55 events per session): is it a release blocker, a documented limit with a recommended session length, or something to fix now?
3. **lod_popping and camera_clipping** have no pixel evidence and a circular gate history. Run B-LOD and B-CC (B-CC needs the lever built first), ship them available but off by default with a stated limitation, or pull them. Your call.
4. **m53** can't be measured until 085-02 builds the shipped modes, and its plan has no offset pixel gate. B-M53 adds one; per 085-00, m53 slips before sync does.

Findings: `_reviews\086-01-label-sync-audit.md` (same text as the commit); checker and data in `_reviews\086-01-work\`.

Several MCP connectors (Gmail, Calendar, GitHub and others) need authorising in their connector settings, and the PDF viewer server failed to connect. This task didn't use any of them.
api-equivalent cost (not billed on Max): $26.90 | turns: 196 | running total across all briefs: $3,146.20
