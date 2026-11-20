# 094-44 — Code's review: the two 4.25 materials are authored and correct by my own byte readback, but the job stopped at Codex's added fresh-process readback (kit FE3 stop, no crash). Nothing of Phase B ran. Nothing was pushed

**Date:** 2026-11-04, about 05:40 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**How it ran:**
- **Resume:** thread `01a14334`, detached, with W4 (the child saw no `GIT_*_DATE`), after a dry run of my launch gates. The prompt is the request verbatim, my context and the implementer block used in 094-43r (`resume-prompt-094-44.txt`, 15,107 B, `CB80B1CB`).
- **Run:** 05:02:34 → 05:31:07 (1,712 s), exit 0, one try. Codex verdict: **NEEDS-DECISION**.
- **Report:** collected byte-identical as `_reviews\094-44-codex-report.md`.
- **Receipts:** `code-receipts-094-44\` (my gates, lists, polls). My readers: `D:\IA_Europa\Temp\CodeReview09444\`. My readback receipts: `D:\IA_Europa\Receipts\CodeReview09444\`.

## 1. What happened
1. **Entry:** FJ1's fast checks pass; the full P9 `main()` passes in 340.97 s (`Receipts\09444\owner-p9-result.json`).
2. **Materials (D2):** the fixture editor (PID 19988, armed kit, `-nowrite`) authored `M_MissingTexture_Checker` and `M_CorruptedTexture_Pink` into the fixture plugin's `Content/Engine425/Materials/` and read them back in that process. It exited normally with 0 pending shader jobs.
3. **Codex then added a fresh-process readback**, declared before launch, to read the saved assets rather than the cached authoring objects.
   - The kit stopped that editor (PID 9616) before its script ran: **`FE3 unreadable UnrealBuildTool command line`**, exit 125, classified `owned-stop`, 0 crash folders.
   - **The stop is the kit doing what FE3 (ruling 094-26b) requires.** The editor's UBT child, PID 9640, was created at 23:52:48.091 UTC and had already exited at the kit's first poll, 23:52:48.358, so no command line was ever read (`read_attempts=0`, `command_line=null`). It lived under 267 ms.
   - **For comparison:** the authoring launch's UBT child (PID 10508), created at 23:44:56.352 and sampled 167 ms later, was `UnrealBuildTool.exe -Mode=ValidatePlatforms -allplatforms`. The stopped child is probably the same query, but that is **not established**. Codex says so too (G529).
4. **Codex stopped there** under "stop on any failed gate". It committed a checkpoint, `9c87836` ("stage 4.25 materials pending fresh readback"), and pushed nothing.

## 2. The commit `9c87836` (13 files, +372/−3)
- **Assets:** the two materials (81,634 B `5222b3ee…`, 99,183 B `b88d8861…`). The Paris copies are hash-equal to the fixture originals, which remain **untracked** in the fixture's detached plugin worktree (2 entries).
- **Authoring script** `tools/europa/create_anomaly_materials425.py` and a py27 test.
  - **Diff against `d7eee97:tools/create_anomaly_materials.py`, read by me:** the graphs and constants are the same.
  - The differences: the output folder; `used_with_nanite` dropped; overwrite **refused** rather than delete-and-recreate (stricter); a `_node` wrapper that raises on a failed create; a checked save; the readback through `used_shading_models`.
- **The 4.25 finders** (`AnomalyInjectorSubsystem.cpp:103-109`): an `#elif ANOMALY_ENGINE_425` branch loads the two new assets into the existing ungated members (`.h:229,232`). `ConstructorHelpers.h` is included ungated (`:23`).
  - ⚠ **Never compiled:** `native_builds 0`, `finder_change_built false`.
- **The three anomaly files are unchanged** from `93613ae`: still compiled out on 4.25, and identical to `d7eee97` apart from the gate lines. So there is no port diff to review yet.
- **Docs:** `job_scope.py` adds the `09444` authority to the three scope lists (the same pattern as 09443r). G528 and G529 are appended (next free was G528). W4 status, journal, architecture, runbook and onboarding are updated. No comments added to source.

## 3. Materials: my own readback of the saved bytes (no editor)
- **The reader** (`uasset44.py` + `matcheck44.py`): it parses the 4.25 package (legacy −7, UE4 518), its name, import and export tables, and every tagged property, then walks the expression graph.
- **Result: PASS on both** (`material-readback-code44.json`). For each material:
  - the six usage flags are serialized `true`: StaticLighting, SkeletalMesh, InstancedStaticMeshes, SplineMeshes, MorphTargets, Clothing;
  - no Nanite name anywhere in the package;
  - `TwoSided=true`;
  - BlendMode and ShadingModel are unserialized, so they are at the class defaults, Opaque and DefaultLit;
  - Roughness is a Constant with R = 1.0; Specular is a Constant at its default R = 0.
- **Pink:** BaseColor ← Constant3Vector (1, 0, 1, 1); 3 expressions.
- **Checker:** BaseColor ← Lerp(A = (0.5, 0.5, 0.5), B = (1, 1, 1), Alpha = Multiply(Frac(Multiply(Add(Floor(Mask R), Floor(Mask G)), 0.5)), 2.0)). Both masks read the same TexCoord, with UTiling = VTiling = 8. 14 expressions. This is `d7eee97`'s graph, node for node.
- **Can-fail:** the same checker fails on three other fixture materials (`material-readback-canfail44.json`). That includes the BlendMode check on `M_Masked`, which proves the blend test reads a serialized non-default value.
- **This is exactly what the stopped fresh readback was meant to establish** (the saved bytes, not the cached objects). It agrees with Codex's in-process readback.
- **What it cannot show:** that `-game` renders them in colour rather than grey. That is G2's can-fail, still unrun.
- **The 5.1 materials are untouched:** both `Content/Materials` assets equal the `d7eee97` blobs (`b616589`, `30b66f0`) in the m51 checkout, the Paris, fixture, Baseline51 and ReGate51 worktrees, and at `93613ae` and the tip.

## 4. Gates
- **GF4** (skeletal and instanced targets): **unrun**. No actor was authored or installed. A draft author and harness exist only in the run folder.
- **missing_texture, corrupted_texture, camera_clipping:** port, G1–G5, can-fails and example frames are all **unrun**.
- **The Paris regression** on D3D11 and D3D12: **unrun**.
- **Defaults:** unchanged, blinking + lod_corruption. camera_clipping is not enabled on 4.25.
- My G1–G5 readers were not needed, because nothing produced frames.

## 5. Safety (my own lists, before against after)
- **Crashes and uploads:** 0 runtime crashes and 0 upload signs; the stopped session's upload check PASS.
- **Admission:** both launches' gates cleared in about 4 s; GA5's bound was never near.
- **Paris:** Content (1,456), Config (18), host and plugin Binaries (16 + 11), the descriptor and the `.uproject` are hash-equal.
  - Plugin Content 5 → 7: the two allowed new assets, on our branch only.
  - `Saved` and `Intermediate` listings unchanged. No Paris file changed outside the plugin.
- **Fixture:** the seven assets, `Saved\Config\Windows\Engine.ini` (`64d45f80`), `Config\DefaultEngine.ini` and `.uproject` are hash-equal.
  - From the editor launches: `Saved\Autosaves\PackageRestoreData.json` was rewritten (96 B, `RestoreEnabled:false`, no packages); `Intermediate\AssetRegistryCache` and `Config` were touched; DDC files were added.
- **Binaries:** 0 changes in any poll. The Paris binary sets are hash-equal; the fixture binaries are unchanged by size and time. No compiler or linker ran. The only UBT processes were the two editor-owned children in §1.
- **Refs and m51:**
  - `m51` passes at start, prelaunch and exit; `master`, `release`, 30 tags and the tag refs are unchanged.
  - Only feat moved, `93613ae` → `9c87836`. Origin is still `93613ae` (`ls-remote`). Both spikes are local and not ancestors of feat.
- **Deletions:** 0 files removed (tree list over `D:\IA_Europa`, the run folder and `E:\IA_Fixtures`). P9's files are unchanged. 0 reparse points.
- **Processes:** none left.
- **Polls:** 11 lines, 2.0–3.4 min apart.
  - My 20 s sampling sighted the authoring editor, its crash monitor and two shader workers.
  - It did not sight the 4 s readback session or either short UBT child; the kit's receipts record those.
- **Disk:** D: 55.9 GiB at exit, 5.9 GiB above the 50 GB floor. 094-43r's run used about 4.6 GiB of D:.

## 6. Push: none
- **The scan of `93613ae..9c87836` is clean** (`_reviews\094-44-prepush-scan.md`).
- **I did not push.** The commit is Codex's own "pending" checkpoint and carries a native change that has never compiled. A step whose code part was not built is not a passing step, so it does not stand alone.
- Holding costs nothing: the office card (pinned to `93613ae`) stays valid, and the next job continues from the local commit.
- **No new office card** was written.

## 7. Decisions for chat
1. **FE3 and a child that exits before the first sample.** The editor's startup UBT query can finish faster than the kit's first poll, and FE3 then stops the session. This can recur at any editor launch in Phase B.
   - **Options:**
     - (a) keep FE3 as is and allow one re-run of a leg stopped this way;
     - (b) have the kit read new owned children's command lines at creation, keeping FE3's stop for a line that really cannot be read;
     - (c) admit such a child when the post-launch checks show no build product changed.
   - **I recommend (b), with (a) until it lands.** (c) weakens the guard.
2. **D2's readback.** Accept the in-process readback plus my serialized-bytes readback (§3) as the readback the request asked for, so the resumed job goes straight to building the finders and GF4? Otherwise the resumed job re-runs the fresh readback under 1(a) or 1(b).
3. **Resume 094-44 from `9c87836`.** Same request, same thread, plus the rulings above. Watch D:'s margin above its floor.
