<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, gpt-6-astra, max, danger-full-access resume, exit 0, 421 s, 01:57:03-02:04:04 IST). Body below is byte-identical to report.md, sha256 1DE8CE769EF2A491A7741EF7E40580CE6E8AF61427F6900B72D262B4CDBA739B. -->
094-43 stopped at the mandatory full P9 entry gate. No implementation, build or runtime session started.

FJ1 fast entry and exit checks passed: protected refs and files were unchanged, all 27 bound binaries matched, and the seven fixture assets and preserved configuration matched. Entry free space was D: 60.83 GiB and E: 111.96 GiB.

The unmodified `owner_p9.py main()` ran once and exited 1 after 7.832 seconds. P9 v3's byte-for-byte derivation passed: 707,775 original rows partitioned into 382,969 retained and 324,806 removed rows. It then failed the assertion in `tools/europa/owner_p9.py:72`, which excludes only the local feature branch before comparing refs with the original baseline.

All 71 baseline refs remain equal; none changed or disappeared. The comparison contains four additional, pre-existing refs:

| Ref | Commit |
|---|---|
| `refs/backup/europa-pre-fv6-09440` | `08fe652` |
| `refs/heads/spike/europa-09442c-mesh-shader` | `740220c` |
| `refs/heads/spike/europa-09443a-depthonly-byname` | `c1193bd` |
| `refs/remotes/origin/feat/europa-port` | `d1cdc7b` |

This is an entry-gate failure, not evidence that those refs changed during this job. P9's later inventory, rehash, toolchain and fsck steps were not reached. Neither the checker nor its baseline was altered.

Stage A: GD3 was not implemented; A-P2, A-P3, A-P4 and A-P5 were not run. There are no new coverage, magenta, shader-cache or runtime-state measurements.

Stage B: not started under the entry stop rule. FZ2, FZ7, the card draft and H1–H5 remain pending; no performance measurements were taken.

Evidence: `D:/IA_Europa/Receipts/09443/entry.json`, `owner-p9-result.json`, `owner-p9-main.log`, `P9/entry/v2-rederivation.json`, `P9/entry/ref-difference.json` and `exit.json`.

Crashes, upload signs and protected changes: none observed. No exe/DLL was built, replaced or launched; no binary archive was created. No bench lever or machine setting changed. No deletion, merge, push or tag occurred. The entry-only configuration permitted no build, query or runtime launch. The remaining work was stopped as explicitly required; no gate was waived.

No new commit was made. Final `feat/europa-port` remains `75ca580bedb72f4db0ac7937107b71336fe866d0`; the admission spike remains `c1193bdc386cc77bccb7c6c756a6f6fe28f195d0`. A ruling on P9's handling of the four existing extra refs is needed before resuming.

VERDICT: NEEDS-DECISION | Mandatory P9 entry failed on four pre-existing extra refs; Stages A and B were not started.
