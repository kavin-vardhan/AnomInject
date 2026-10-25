# 094-11 A5 probe and readers

Authority: EK1-EK3, 094-10 request, EJ Next 1-8. Production plugin source is unchanged. No runtime/authoring gate is claimed by this document.

| Instrument | Reach and gate | Input / independent observation |
|---|---|---|
| Host WorldProbe | A3, A5, A6, A8; R3, R9 | Native world delegates, concrete subsystem arrays/CDOs, independently counted world epochs; catalogue/pool observations |
| Host ProbeDriver | A5-A8; R3-R9 support | Controlled apply/revert/pause/travel, original component flags/material identity, forced GC weak observers, dormant objects, owned bench components |
| Screenshot driver | A7; R5/R7 picture support | Native engine-written PNG plus request/completion/world receipt, one pending; does not call disabled capture |
| gate_readers.py worlds/gc | A5/A6, R3 | All five classes, two PIE worlds, three balanced lifecycle cycles, pause/TimeOnly/resume, multiplicity and positive EditorPreview; no empty pass |
| compile-out/descriptor | A3/A4 | Original engine startup log, generated definitions, native actions/target products, external listener/registration observations |
| picture/events/screenshots | A7 | Twenty unique clean PNGs, frozen ROI, >=3 events for each of four IDs, two restored PNGs, exact state and acknowledgement joins |
| admission/catalogue | A8/R9 | Exact ordered release catalogue/defaults/pools/seed sequence, unsupported requests, package-load log and teardown state |
| edges/release-edges | R4/R5 | PNG-derived activity versus independently delivered labels; release per-edge pixel observations, explicit zero-offset arbiter |
| speed | R6 | Three matched repetitions, raw wall/nominal capture-tick spans, frames, pace and instrumentation settings |
| pairing/control | R7/R8 | Frame/family/mask association, original component/owner/co-user state, census timings; redacted protocol transcript with real image/frame counts and close/reconnect |

The host copies' Source/EuropaPhaseAProbe files are authoritative runnable host sources; matching versioned templates are in tools/europa/host_probe. Their hashes are part of every A5 build binding. The baseline includes no feature Compat header. All behavior is non-Shipping and dormant without the explicit two flags.

`Europa.Probe snapshot <label>` records subsystem and tagged EA_ actor component states. `prepare` selects EA_Camera and disables temporal AA/dynamic resolution/motion blur; use a 1280x720 PIE/game viewport. `screenshot <unique.png>` requires the game viewport's world. `apply <id> <args...>`, `revert <id>`, `revertall`, `pause 0|1`, `travel /Game/EuropaPhaseA/EA_Travel`, `gc`, `lifetime`, `preview`, `auto-seed 4242`, `auto-fire`, and `script <absolute-D-json>` are controlled commands. A script is an array of command/args/wait_frames/expect_success objects; every unexpected outcome stops that script with a failure record. Deliberate refusal controls set expect_success=false so their images are still taken. Travel ends that script; restart one in the destination world after verifying its identity. No arbitrary console execution is accepted by this driver.

Enable `-EuropaTickAudit` only on lifecycle legs. R6 omits `-EuropaPhaseA` and `-EuropaTickAudit` altogether. Screenshot/GC driver data is host evidence, not a replacement for the stock capture labels/masks or a 4.25 label-sync claim. Runtime mutants (admission, lifetime, UPROPERTY, timing, renderer and transport) remain the subsequent jobs' work. Synthetic reader can-fails cannot substitute for buildable, effective runtime controls.

Readers accept documented JSON payloads using their function parameter names (`python gate_readers.py <gate> --input <payload.json> --output <result.json>`). File SHA-256 binds the payload. Picture payloads name actual PNGs; set artifact_dir to a fresh owned D directory for absolute-difference PNGs and a contact sheet. World/GC/screenshot payloads contain the unmodified probe JSONL rows. Do not hand-enter measurements or infer success from command_result alone. Raw supporting logs, action exports, pixel files and independently qualified CaptureBench readers accompany each gate result. A5 self-test source is reader_self_tests.py; its generated inputs are clearly synthetic.

The existing capture reader's label-pixel and black-frame self-tests, change oracle and annotation consistency tests are rerun on synthetic inputs. Their own semantics remain unchanged. For zero-edge qualification, pass the actual per-edge detail to release_edge_details; OFFSET notes and unassessable events are failures at this boundary. Never use the existing reader's exit=0 as the zero-edge proof.
