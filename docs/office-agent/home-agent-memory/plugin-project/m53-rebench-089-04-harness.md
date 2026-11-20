---
name: m53-rebench-089-04-harness
description: "The m53 re-bench harness _reviews\\089-04-* RAN (089-04, 2026-09-30, 47/47 on DC2) and was re-read offline by 089-04b; frozen bytes still intact; G-MODE fix lives in 089-04b-gmode.py."
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-09-30T03:34:20.223Z
---

089-02b built `_reviews\089-04-*` (README first); 089-04 ran it (`089-04-window.py night --commit`) on DC2 = exe 6B671987 +
utoc 45E2FA86: 47/47 accepted, 10 NOT-RUN-PREMISE (no normal MainWorld host), restore to m53 S1, postflight PASS. Banks
`D:\IA_BankOverflow\M53DC2_*`, as-built report `089-04-evidence\run\report.*` (never edit). The 089-04 Code session died on
ENOTFOUND after the harness finished; 089-04b was cut by a PC shutdown and resumed; journal on m53 `620e3cb`
(`docs/sessions/2026-09-30-089-04-m53-rebench.md`).

089-04b findings (offline): G-MODE's two family mix-ups (overall P-ORC gate; premise lock pooled both recipes) fixed in
`_reviews\089-04b-gmode.py` (selftest 12/12, re-eval PASS 4/4) - the frozen 26 files untouched; the normal oracle PASSES on
its own unlit recipe. RD5 "end -4" is E1's post-label reference holding the 3 late frames; pixels read +3 (`089-04b-rd5.py`).
MainWorld uv hosts SM_Ramp2 / RoomBuilderSquare are Nanite (no mask).

**Why:** chat's 089-04 ruling expected "normal INSUFFICIENT" and an RD5 label question; both premises were refuted by
measurement, and the NEEDS-DECISION items (G-MODE-N PASS, RD5 on the pixel reading) wait for chat.

**How to apply:** do not re-run legs or edit frozen bytes unprompted; re-evaluate from banks via new `089-04b-*`-style
modules that hash-check the freeze. Related: [[env-refuses-dbghelp-dll]], [[m53-s2-0-harness-boundary]].
