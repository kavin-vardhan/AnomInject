# 093-14 freeze: drop-aware camera donor and full PIE rerun

Product remains 9781031, initial docs head 0bc6e9f, remote 1bf7054. Preflight rehashed
both candidate banks and archived copies identically, including Lyra fixture DLL
017ca90bdffd2b79b30fa7a1e262c9f747c0070aece0608ba76177e85455edb3. Restored host
files/content and 5,764 stage files match. Git date overrides are absent.

Predictions 093-14-camera-donor-full-pie.md freeze AB1 before any assessment/leg.
The original _r9313 unavailable-control result is retained. Attempt 1 selects the
last written sample before each switch, requires distance <=10, and refuses an
unavailable control. No product, estimator or tolerance changes.

AB2 withdraws identity inheritance and schedules the full PIE matrix on 9781031;
only the archived J1/pre-F1 PIE controls stand. AB3-C reads exposure exclusion from
DRAWN SUMMARY, without a JSON field. AB3-D is an open post-J2 item: check whether
m55 change evidence validates under camera motion. Evidence is _r9314. Execution,
controls, commit timing and final restoration will be recorded at closure.

Freeze 741e8a1836754116fd890ed095542d46cf185011 was committed at 03:14:27 IST.
Retained Stack re-assessment started at 03:15:18.604 IST, after the commit. Both
input sessions and the original failed outcome rehash unchanged. Donors 56/296
are four samples before the cut/blend; each control has 44 measured rows per axis.
The cut copy reads X=.920863/Y=1.077844 and the blend copy X=1.044657/Y=.960920;
both reject. This completes donor repair attempt 1, with no second attempt.

Fresh Lyra reference and active sessions 032444/032822 pass the same cut60/blend300
specification. Both read 990=984+6 owner_render_missing drops, samples 57-59/297-299,
with zero accepted stale/frame/world mismatch and admission counters 0/0. Active
full scales are X=.9999999404/Y=1, moving scales X=1.000097912/Y=.999884435, n44.
All four visible events have zero onset/recovery error and all 714 labelled rows
have measured masks. Both active events continue, ending at samples 208/448 with
pie_end_settle recorded. Donors 56/296 reject: cut X=.937729/Y=1.058824; blend
X=1.033333/Y=.970948. Both camera legs record 16 extent_mismatch change pairs.

Own harness repair before engine launch: the fresh string-audit wrapper selected
J1 instead of the exact eff797c baseline frozen in 093-13. The original unknown
boundary-string result is retained. Attempt 1 restores the frozen per-host input
baseline and validates its manifest; scanner, classifier and sentinel controls
are unchanged. Both encoding scans then have zero unresolved findings and both
unknown-string controls reject. No product/binary change or engine retry.

The ordered fresh PIE PF gates pass on 9781031. PF-1 has 360/360 writes, zero
visible rows and zero generic/texture applications, with four off-screen refusals;
the retained four-fire/96-false-row controls and generic-apply mutant reject.
PF-2 passes all twelve fixture/constraint/size combinations, each with 96 measured
rows per axis and both 0.96 mutants rejected. The retained exact f608d56 replay
still reads X=.8130053613/Y=.8130053403, n96, and rejects. PF-3 UV/proxy use the
original 1280x992 viewport setup and both compare with 0.0 px maximum box delta,
identical events/targets/visible_positive and zero edges. PF-4's letterbox and
PF-5's locked FOV pass; PF-5 observes requested/effective70 versus old-route cache90.

Both fresh PIE resize legs reconcile 360=358+2 owner_render_missing drops at
samples98-99, with zero stale acceptance and four zero-edge visible events.
The DRAWN SUMMARY exposure-exclusion reading is 2304px for each. No summary key
was added. Full ledger completion, pairing, dedicated TSR, cook and staged results
remain to be recorded after their ordered execution.

At 06:18 IST, all 42 fresh candidate PIE ledger cases are complete and pass. The
four exact archived-DLL PIE controls remain the only carried matrix results. All
four native UV/proxy legs write 1152/1152 with four zero-edge visible events and
ten rejecting controls apiece; originals rehash unchanged. All four synthetic
alignment legs pass identity/membership/barcode checks and their three mutants.
Historic pairing C/P/N, native100/native50/synthetic100/synthetic50: Stack
56/0/0, 55/0/0, 57/0/0, 62/0/0; Lyra 57/0/0, 59/0/0, 56/0/0, 57/0/0. Every shifted
copy rejects. Every 50% leg covers all 80 captured samples with internal640x496
inside output1280x992, consistent picture/mask/label sizes, and rejecting full-size
log/wrong-width controls. Pairing part3 remains retired under AA4; dedicated TSR50
still requires both axes n>=20 and scale within one percent.

Both admission probes pass 360/360, v4/e0, O/N=0/1, with counter/application
controls rejected. Fresh reader session033532 prints I1.11 PASS v=4 c=0 b=1000 #0
under Python3.10/3.13. Retained null-mask FAIL and Stack/Lyra box PASS/FAIL lines
match on both interpreters. All eight fresh reader mutants reject, originals
unchanged. The numbered twenty-frame contact sheet was inspected locally.

The first dedicated TSR50 leg launches at 06:26:56.777 IST; session062725 completes
360/360. Its positive box result is n96/axis, X=.9961088900/Y=.9944751381, all96
labelled rows paired, and both 0.96 controls reject. All360 pictures/96 masks/rows
use1280x720. All294 logged mask passes show internal640x360 with zero bad extents.
The frozen full gate FAILS because assess_z2.reduced assumes a target-mask request
for every colour row, ID=(1<<61)|(session_index+1). Product source instead increments
TargetMaskOwnSerial only when a mask is armed and stores the actual sample mapping.
This session has serials1-96; serial1 serves sample29, not0. The reader reports264
missing expected IDs (nominal samples96-359). Pairing's forced per-sample probe
made that assumption hold there; it does not hold for this ordinary sparse-mask
capture. stop-readings09314.json preserves source/log/reader hashes and readings.

The full gate stays FAIL. No product or reader change, relaxed predicate, new leg
or retry follows. AB1 authorizes only the camera donor change; fixing this receipt
matcher needs a new ruling. Lyra dedicated TSR, cook, all38 staged matrix cases,
staged PF/camera/admission and push remain NR. This is NEEDS-DECISION, not a measured
wrong box or a new crash. The first stop-reading writer hit Windows cp1252 decoding
on UTF-8 source; one explicit UTF-8 read repair produced the receipt without changing
any gate or original input. Packaging/capture overflow scripts were prepared but
never executed; no storage relocation, cook or metadata repair occurred.

Restoration review owns33 Stack and34 Lyra generated files, with no unknown path
and no owned engine alive. Hosts return to23098fa/caa68c6 with201/183 original files
and2748/2879 content files unchanged. Final stage, archive and commit receipts follow
in the closing report. The product and helper bytes remain unchanged from9781031.

Closing archive routing repair: the logical D:/IntrusiveAnomalies/_binary_baselines
root is a junction onto E:/IA_BuildCache/_binary_baselines. The first archiver checked
D: free space, then wrote its new partial archive onto E:. Only its owned PID42388
was stopped after checking creation time and exact command line. The lowest recorded
E: free space was 55,376,867,328 bytes, above the 53,687,091,200-byte floor. All26,576
partial payload files were inventoried in archive-route-attempt1.json. Automatic
approval review rejected the receipt-scoped removal as "blocked by policy"; no
deletion was retried. The partial archive remains explicitly incomplete and is not
the final archive. No source evidence, prior archive or host file was removed.

Archive routing attempt2 uses physical D:/IA_BankOverflow and an OVERFLOW-UNQUALIFIED
name. Its capacity check resolves the destination parent before checking space and
reserves two GiB for metadata. Source snapshots and the failed attempt's script and
receipt remain preserved. The full rehash, report and archive-index addition are
recorded by the closing receipts; qualification remains NEEDS-DECISION with no push.
