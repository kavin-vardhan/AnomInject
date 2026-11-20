# 078-15 Codex takeover and prediction-only dispatch

2026-09-07. The successor Codex session took sole orchestration ownership after reading ToCodex/CURRENT.md, handoff 11 and the linked read order. No new Chat response was present; latest remains 078-14-chat-campaign-evidence-review.md. Both completed offline checks remain accepted with their stated limits. Drafting is already authorized by that review; no further owner design permission was requested.

## Live preflight

- Plugin m51 HEAD 46139fd3ef20fab90d0768492a987a9952cceb62; master/origin-master ac13700d99fe116056f5aa11611f8931a563a3f2. Source/Shaders diff against 6713f510166a0d9458387caaae711779b07fad64 empty. Tracked-clean; ten owner handoffs untracked.
- CaptureBench HEAD 472a409dc70ae78d01a38d08fc3bddb15fe41c58, tracked-clean, seven existing untracked tools. Lyra detached-clean 4a4bfcc2cdbade6bbeb99494e540952847ae9b63.
- Rehashed A archive B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8, candidate archive and staged exe D50DDE789A94933182DC6B3FA74811F8BB57DC1F8332B4AC88ACE47337BD4796. All five staged container hashes match handoff 11. No swap or runtime action.
- Rehashed all five original labels.jsonl inputs and both checker/oracle sources against 078-15-offline-evidence/provenance.json: all seven match. No analysis/checker rerun.
- Queue and active-brief inventory empty; 078-15 mailbox number unused. Previous report 078-14 read fully; its GREEN applies only to docs. Prior Code session measured 80% and was not resumed.
- Local watcher.ps1 SHA256 79CDABE53E19F2F9D972502E18D5EDF845F90DCD9F170360DF7B56DBDF6030ED and run_brief.ps1 4B09E98F456C945BCE161434EEBD244A66FABD881813429A200A875C48C862CF equal their E:/MailBox/watcher canonical copies. Launcher and full scripts read; no config/script mutation.

## Recovery and dispatch

The sandbox denied Win32 process inventory and git ownership differed under its account. The approved read-only checks outside the sandbox established the repo state. A second approved command rechecked zero watchers, zero Code CLI runs, zero StackOBot/build/shader processes, empty queue and no active brief, then launched the existing watcher hidden under the owner account. No owner application was inspected for content or manipulated.

Watcher PID 22072 started at 07:34:56, produced a fresh idle heartbeat and resolved Claude Code 2.1.260. Source brief _reviews/078-15-identity-lifecycle-predictions.brief.md was published exclusively as UTF8 without BOM into to_code/078-15-identity-lifecycle-predictions.md. Source/queue SHA256 both CB54F390B7A6C16FF265988DCBD11AD60C780DA5DCA0541A05027921FF6A20BE.

RUN at 07:35:42; fresh Code session <uuid>, claude-opus-5, xhigh, SESSION new-default. This is the one in-flight brief. Codex retains monitoring/review ownership; no successor handoff is offered while it runs. Code initially hit the historical Bash shell failure, then switched to PowerShell and verified baseline before reading governance.

Released writes are limited to the new identity/lifecycle/geometry prediction and journal, a current CLAUDE.md block, and a dated contract append. Existing prediction files and the single gate manifest stay unchanged; the proposed crosswalk is not executable. Source, instruments, fixtures, gates, builds, captures, staging, merge/push/tag remain held. Code must return NEEDS-DECISION for exact-draft review.

## Independent review leads

Direct source read in this successor confirms: AnomalySveCapturer.cpp:22-69 stores bare IDs and consumes FIFO with family number only logged; AnomalySceneViewExtension.cpp:51-69 publishes a key by family number; AnomalyMaskSceneViewExtension.cpp:102-133 consumes the current shared pending mask queue on RT. These are separate identity paths.

Local UE SceneRendering.cpp:4527-4528 flushes world end-of-frame updates before publication; :4553-4570 assigns family.FrameNumber from the scene counter (or GFrameNumber without a scene); :4590 invokes BeginRenderViewFamily on GT, then copies families into renderer structures. LaunchEngineLoop.cpp:5363/5568 puts GFrameCounter increment after GEngine->Tick. Raw scene-family/RT/game counter equality is not justified by these facts; same-domain sampling must be explicit.

CaptureCurrentFrame at subsystem:4181-4246 admits SVE arms without a bDisableWorldRendering check, but the capture FSM at :688-713 arms only in capture phases, not settle phases. Thus a withheld tick count cannot simply be asserted to equal requested capture opportunities. GameViewportClient.cpp:1696-1707 gates family rendering on bDisableWorldRendering, player views and platform permission. These are source expectations, not exercised-lever evidence.

LabelWriter.cpp:105-108 and :493-501 still resolve actor projection when building the label record; :537-540 still ignores JSONL append success. None of this is fixed by the prediction-only brief. Review the eventual exact draft against these source paths and the request-ID joins before requesting Chat review.

Further source-backed recipe correction: AnomalySveKeyRing.cpp:17/24 registers IAI.Capture.SVE.ForceMiss and IAI.Capture.SVE.ForceMissPhase; a search of Source finds no IAI.Bench.ForceMiss alias. Older predictions/manifest use the latter spelling. Any future M-R8b command must use the actual registered controls; no gate or source edit was made here. LookupKey also reads the matching entry without consuming/invalidation, so repeat/family identity needs an explicit one-time consumer policy in the proposed binding.

## Completed boundary - 2026-09-07

The in-flight description above is historical. Code completed at 07:59:48 with NEEDS-DECISION, documentation commit a031b469315daa7815efd5a3fc732e1eca556525. Codex read the full report, all 664 draft lines and the three other changed docs, rechecked source and pins, and recorded seven blocking findings in _reviews/078-15-codex-draft-review-and-request-to-chat.md. Documentation delivery verified; design NOT approved. Source/Shaders and manifest unchanged; staged candidate rehashed unchanged. No runtime work occurred.

At 08:16:12, exactly one watcher PID22072 remained; zero Code CLI, bench/build/shader processes, queued or active briefs. Idle heartbeat fresh at 08:18:29. Code session <uuid> ended at measured532244 tokens/53%, below its separate70% rollover boundary. Chat review request is prepared on disk; no new Chat response or external notification. Handoff12 carries exact state/read order/next action. No successor brief dispatched; outgoing Codex will remain inactive after takeover.
