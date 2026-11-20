# NAME: 080 m52 stuck low mip
# MODEL: claude-opus-5
# EFFORT: xhigh

# 080-02 — m52 `stuck_low_mip`: implement per the approved plan, gate on both fixtures. Resume session 913cb70f (52%).

## 0. Rulings on 080-01 §8 (chat, 2026-09-20)
1. Pool: `GAutoPool` yes; `GAutoPoolDefaultEnabled` NO until `G7` yields a number (owner decides default-on after; recommend default-on if ≥ 25 % of visible targets are eligible on MainWorld).
2. Fixture: (a) StackOBot MainWorld + Lyra. No `CB_MipLevel` cook now; note it as the future bench fixture if MainWorld yield is too low to gate reliably.
3. `label_schema` stays 2; the new keys live inside the `stuck_low_mip` anomaly object only; m51's schema-3 bump absorbs them later (record in the schema note).
4. `MaxCoAffected` default 0 — keep.
5. S2 multi-node disclosure — Tier-2 only, not built now.
6. Hold ALL eligible textures of the target.
Lever L4′ (`NumCinematicMipLevels` + `UpdateCachedLODBias()`) ACCEPTED with its five failure modes as detections (each must have a label/telemetry field and a gate that exercises it). Sharing policy S4 (per-texture gate on the visible set) ACCEPTED. `AnomalyState` with `IsCurrentlyAnomalous()` = mip measurably below baseline ACCEPTED. Console names follow the shipped convention. Gates are packaged-only (Development, not Shipping — the capture build) as the plan declares.

## 1. Order
1. Predictions already exist — add AMENDMENT 1 with these rulings and the predeclared readings for G1–G11 (numbers where the plan left them open only where a value can be predicted; G6/G7 get their acceptance thresholds: G6 hitch = max frame time with the anomaly ≤ baseline + 2 ms at 1280×720 on the bench recipe; G7 yield printed, no threshold).
2. Implement per §9 file-by-file: registry entry, `AnomalyState`, texture pick (GetUsedTextures, streaming-2D only, VT ⇒ NOT_APPLICABLE), S4 visible-set gate, hold/restore on every exit path (FinishRun, cancel, actor destroyed, level change, PIE end), telemetry keys (`resident_mips`, `full_mips`, `forced_mips`, `held`, failure-mode flags), dashboard toggle, console commands, client-readme row + field reference, changelog line.
3. Build (Development), stage, hash — same discipline as m44–m49 (archive the exe, quartet hashes).
4. Gates, predictions first, in this order: G1 unit/registry; G2 can-fail (non-holding variant ⇒ `held:false`, `observable:false`, no labelled frames); G3 hold-through-span on MainWorld under a camera sweep that would normally stream the mip back; G4 restore-on-every-exit; G5 onset (first labelled frame has resident < baseline; m44 ONSET gate satisfied by construction — prove it); G6 hitch; G7 yield on the visible set (MainWorld and Lyra) — print eligible/visible targets per frame over a 600-frame session; G8 Lyra hold + restore; G9 schema additive check + client-readme field test; G10 both tick orders; G11 verifier consistency read on the m52 session with the 079 tool (readings only; expected: CONSISTENT/OFFSET-NOTE, zero NO-TRACE; if a NO-TRACE appears, that is a FINDING about m52's labels — report it, do not tune).
5. Docs: journal, gotchas (the L3 packaged no-op trap gets its own G-number; the streamer cancellation predicate another), CLAUDE.md status block, `docs/anomaly-catalog` entry (or wherever the catalogue lives — cite it).
6. Commits on the branch, Conventional Commits (`feat(stuck-mip): …`), pushed. NO merge, NO tag. Report per-gate with exact lines, the G7 yield table, hashes, and `VERDICT: GREEN` alone on the last line (NEEDS-DECISION if any gate misses or a failure mode fires unexpectedly — with numbers).

## 2. Rules
No Source change outside the plugin; never kill by process name on the owner's box (kill by PID you started, assert idle first); no office data; one exe archive per build; predictions before results; no threshold tuning after a reading.
