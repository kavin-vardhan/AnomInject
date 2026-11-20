# 093-13 exposure-exclusion repair and camera-control scope stop

Requested 2026-10-15; execution crossed midnight into 2026-10-16 IST. Worktree
E:/IA_BuildCache/_r933/AnomalyInjector, fix/followup-capture. Initial head 7049614;
remote 1bf7054 remains unchanged. No date environment overrides, natural-cause code,
engine edits, new module dependencies, merges, tags or pushes.

Design/predictions ef03f36e07bb68c2e5ffd9c3f28fcb5820896836 committed at
00:11:01 +05:30 before product edits. One product commit,
978103163c0a1839bd6948614818b73f07674c72, at 00:14:14 +05:30, changes only the
capture subsystem cpp/header: 17 insertions, 10 deletions. ResetExposureExclusion
clears all five members at every reset; allocation follows actual array length,
with shape-change clearing retained. Source remains frozen at this product commit.

Preflight rehashed the eff797c baseline banks and archive identically. Hosts matched
201/183 original files, 2748/2879 content files, heads 23098fa/caa68c6; live staging
matched all 5,764 hashes. report-093-12.md preserves the preceding report. This
baseline is retained for identity, not automatically transferred to the new binary.

The CPU isolation harness compiles actual extracted old/new fold and owner-reset
code. Old owner_same_size, owner_clears_all and actual_count fail as predicted;
new passes all five cases, including repeated_event and same_count_new_shape.
The existing 69 pure-suite steps, 9 family cases and 5 ownership mutants pass.
All eight strict/normal Editor/Game builds pass with zero errors/warnings. Strict
checks compile 71 header-isolation TUs each, with no PCH/unity lever problems and
exact Build.cs restoration. Binary audits and full helper command-name checks pass.
ASCII/UTF16 scans leave zero unknowns; Lyra's one boundary string pnullptr is the
source filename's final ASCII p joined to an unchanged UTF16 nullptr literal.
Both injected unknown-string controls reject. New EXEs: Stack D8D30951, Lyra DFE6F164.

First engine launch: Stack camera reference, 01:13:04.463 IST, after idle checks.
Both specs are identical to the retained 093-12 camera specs. Both new runs pass
the cut at 60 and blend at 300 without an assertion. Each reconciles 990 requests
= 984 pictures + six owner_render_missing drops (57-59, 297-299), zero accepted
frame/world mismatches, no pending requests. Admission counters are 0/0 in each.
Reference has 16 change-stage extent_mismatch pairs, four before the cut; this is
the requested reading, not a gate. Final exposure-exclusion log count is 6,050 px
in both runs; no ExposureExclusionPx JSON field exists.

The active leg has four visible events: [29,208], [269,448], [509,688], [749,928].
Every onset/recovery error is 0; all 714 labelled rows have measured masks. Full
X/Y scales: 0.9999999403953554/1.0, n714 each. Moving [301,344] scales:
1.0007299848553448/1.0001271307833637, n44 each. Both switch-time events continue;
annotation transition reasons record pie_end_settle, and logs show four exact
restores with zero residue. Identity self-control rejects unchanged pictures.

STOP: the unchanged camera reader cannot construct its stale-projection control
because it hard-codes sample 299, one of the named pre-blend drops. Sample 296 is
the last written pre-blend sample, but no substitute control was run. The full
camera gate is FAIL/incomplete despite the primary measured floors passing. A
reader change is explicitly outside 093-13 except AA4; AA5 also stops a new-binary
camera gate failure. This is a harness scope decision, not a measured new label
or picture disagreement. Do not retry the fix or claim the rest inherited.

Lyra camera, delayed/resize reach, healthy/UV identity, dedicated TSR50, cook and
every staged gate are NR on this binary. The prior 46 PIE matrix passes and PF-1
through PF-5 remain eff797c evidence; identity inheritance was not reached.

Harness repairs, each first attempt: FString unary dereference added to the CPU
shim; omitted descriptor added to scratch lever source bundle from exact product
and helper metadata; closing source hash compared to the committed Git blob rather
than CRLF checkout bytes. Original failed outputs remain in _r9313/harness-repairs.
CPU environment bootstrap also emitted its existing vswhere-not-recognized line;
the explicit MSVC compiler resolved and compilation succeeded. No compiler or
security setting was changed. Camera donor failure is retained without a repair.

Hosts and staging were restored by receipt: original heads/files/content match,
all 5,764 stage hashes match, and no owned engine remains. Twenty-one new build/
crash-config files and five installed roots were removed only after ownership and
hash review. No foreign process was touched. G486 records the reset lesson.

Archive location for closure:
D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-exposure-09313-9781031-D8D30951-DFE6F164-UNQUALIFIED.
Final archive/re-hash and report receipts live in E:/IA_BuildCache/_r9313.
NEEDS-DECISION: authorize a drop-aware stale-projection donor rule, or specify the
required control under W2. No reader change, replacement negative, further leg or
push is performed in this job. Full report is in the shared relay run folder.
