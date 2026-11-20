# SESSION: new
# EFFORT: xhigh
# NAME: 069 m47b auto-exposure

SESSION 069 (thread A), BRIEF 26 — symptoms (2)/(3) attribution: AUTO-EXPOSURE, measured. Fresh side session: bootstrap from CLAUDE.md (shared-tree rule clause 4), the 069-25 report (_mailbox/to_chat/069-25-m47-unblocked-rulings-and-run.report.md), journal 069 §17. Codename discipline. Do not adopt as mailbox default. Checkout: master (93c2b65 == origin), leave europa-e1 as found afterwards. Verify the staged exe (F309D836) before any leg.

VERDICT ON 069-25: accepted — the strong negative, the counter-scope fix (G232), the pixel-based packaged claim, the reverted asset churn, the A44 catch. ND-A (skeletal cold-DDC leg): FUTURE — it is an editor-only phenomenon by construction (packaged builds carry precompiled shaders) and needs a new frozen fixture + cook; not this week. ND-B = (iii): test auto-exposure FIRST.

WHY AE IS THE PRIME CANDIDATE FOR (3) AND POSSIBLY (2): every bench leg forces `r.DefaultFeature.AutoExposure 0, r.EyeAdaptationQuality 0` (run_leg.ps1:187, run_leg_editor.ps1:78, verify_lastrundir.ps1:61); the plugin itself never touches exposure (no EyeAdaptation/AutoExposure reference in Source/). The owner's editor runs the game's defaults (AE ON). A large magenta/checker surface appearing changes scene luminance; eye adaptation then re-exposes over a burst of frames — "whole picture dark for a burst, then recovers" — and the target itself can read dark on its first frames while exposure settles. The DELIVERED build also runs the game's defaults, so this is NOT editor-only by construction — the owner may simply not have looked at packaged output for it.

TASK A — MEASURE (both targets):
 A1. Editor leg, AE ON (run_leg_editor.ps1 -AutoExposure 1), corrupted_texture + missing_texture events, 90 frames: per frame whole-frame mean luminance and target-region luminance (m47_lum_table.py, mask discriminator on), plus `r.EyeAdaptation.*` state echoed. Prediction: after each fire, whole-frame luminance dips for a burst of N frames and recovers; the target's first-frame luminance is lower than its event mean; the m43 mask stays correct on every dipped frame (geometry drew fine — blackness is exposure).
 A2. Control: same leg AE OFF (today's default): no dip.
 A3. Packaged leg with the AE-off cvars REMOVED (i.e. the game's own defaults — say what they are: r.DefaultFeature.AutoExposure, r.EyeAdaptationQuality, the project's DefaultEngine.ini exposure settings): does the dip occur in the packaged build? This decides whether the client would see it.
 A4. Does the black-frame gate (threshold 6.0) fire on the dip? Report min luminance during the dip vs the threshold. It should NOT fire on a legitimate exposure dip; if the dip goes below 6.0, say so — the threshold is then wrong, not the dip.
 Report the dip depth (min/mean luminance), duration (frames), recovery, per event type, per target/leg.

TASK B — RULING PREP (no product change without chat):
 If AE explains (3) (and (2)): this is the GAME's rendering, not a capture defect — the dataset should look like the game. Proposed handling (do not implement beyond docs/counters): (i) a labels-side reading per frame `exposure_dip: true|absent` derived from a whole-frame luminance drop > X% vs the rolling mean of the previous 8 frames (additive key; threshold from A1), and run_summary `frames_exposure_dip` — so the consumer can filter; (ii) client-readme: one paragraph "after a large texture anomaly the game's auto-exposure re-adapts for a few frames; frames are marked"; (iii) no forcing of exposure by the plugin. If A3 shows the dip in packaged, (i)+(ii) become m48 for the Bates drop (small); if A3 shows none, (ii) alone as an editor-only note. Say which, with numbers. If AE does NOT reproduce the dip in the editor (A1), report and propose the next discriminator (Lumen surface-cache invalidation was named).

Commit only docs + the harness changes (journal 069 §18, ledger, gotchas if earned — G233 exposure dip, named not asserted until A1 shows it). NO source change in this brief. Leave the checkout as found. REPORT: A1–A4 tables, the per-symptom verdict, the m48 recommendation, master tip. STOP.
