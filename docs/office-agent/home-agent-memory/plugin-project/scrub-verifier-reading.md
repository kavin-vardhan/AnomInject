---
name: scrub-verifier-reading
description: "The codename scrub verifier echoes its term list and flags short identifiers; keep only VERDICT, locate hits by bisection on return code"
metadata:
  node_type: memory
  type: reference
  originSessionId: <uuid>
  modified: 2026-11-13T12:31:34.125Z
---

`CaptureBench/tools/m36_scrub_hostname.py` (untracked in CaptureBench) prints its self-test mapping, which names
real titles, before the VERDICT line. Its abbreviation rule also matches short identifiers: a two-letter Python local
in the m55 change-oracle selftest of `tools/verify_capture.py` read as a term until 095-05b renamed it (338e4cb).

**Why:** pasting its stdout into a receipt or report leaks the very names the codename invariant forbids; and a
"TERMS PRESENT" on an unchanged file is often a false positive that blocks a push.

**How to apply:** keep only lines starting `VERDICT` (receipt script `code-receipts-095-05b\prepush_scan_05b.py`);
to find a hit, scrub line chunks and bisect on the return code, printing line numbers only
(`code-receipts-095-05b\scrub_bisect.py`). Gotcha G578 on `fix/m56-capture-continuity`.
