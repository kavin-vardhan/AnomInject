# Staged census proof audit, before final staged reruns

The numeric log checker conservatively rejects any unrelated plugin line within the count block. It caught an asynchronous `LogAnomalyCapture` telemetry line interleaved in `census-staged-stacks-all-02`. The final staged harness sets that category to Warning for this passive check. The checker remains unchanged; no category exception is added. This verbosity setting is process-local and restored by process exit.

Reviewing the capture metadata also showed the first 120 staged images span more than 120 engine ticks: the old census began before the null ended. The prior scene reader could therefore include diagnostic-time images in its null. Those staged legs are invalid pixel proof even though their counts and state fingerprints match.

The new reader independently requires the logged census begin/end ticks to lie after labels row 119's engine frame and before capture end. Labels are used only to map captured images to engine time; the scene-change test still reads the actual pixels. Existing null length, reference index 100 and null maximum + 0.05 MAD are unchanged. The corrected schedule starts capture at DEFER 780, snapshots at 1120, runs census at 1124, and snapshots again at 1210. Each run must prove the ordering from its recorded metadata; the schedule alone does not authorize a pass.

The old four PIE results and two staged results are saved as `census-result-without-timing-gate.json` before rechecking. The PIE results must continue to pass; both old staged results must fail the added timing gate. This supplies a concrete can-fail for the timing check. Final staged legs are rerun in full under the corrected schedule, without moving ROIs or changing pixel tolerances.
