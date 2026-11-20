---
name: g118-cooked-placeholder-token
description: "The staged BenchGate build enforces the placeholder control-server token TESTVALUE123 while the project ini carries a rotated 64-char one — G112's guard checks the wrong artifact; filed, deliberately NOT fixed"
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-08-19T08:52:49.860Z
---

**The staged build (`101AFEA4` = m25) enforces the control-server token `TESTVALUE123`.**
`D:\IntrusiveAnomalies\StackOBot\Config\DefaultGame.ini` carries a rotated 64-char token, but the
build was **cooked before the rotation**. Its own startup log states it:
`=== Control server token: TESTVALUE123 (from DefaultGame.ini [AnomalyControlServer] Token) ===` —
and the parenthetical means the **cooked** ini, a different file from the one on disk.

**Why it matters:** G112 installed a placeholder guard, and `verify_lastrundir.ps1` applies it to the
**source** ini, which is clean. **The guard fires on the artifact that enforces nothing and stays
silent about the one actually listening on `ws://127.0.0.1:8077`.** `TESTVALUE123` appears in repo
history, so anyone reaching that port on a stale cooked build is authenticated.

⛔ **NOT FIXED, deliberately.** The fix is a re-cook **and** re-stage, which wipes `Saved` (G92 — the
bank must be refreshed first) and **would replace the exact binary every m25 result is measured
against**. It must not ride inside a measurement turn.

**How to apply:** read the enforced token from the **running process's log line**, never from the ini
you edited — `ws_scoping_echo.ps1` does this and prints source-vs-enforced side by side. Until closure,
treat any WS work against this build as running with a known-public token. Found while collecting H4's
corroborating A48 echo → [[h4-preflight-halt]]. Gotcha **G118** in `docs/gotchas.md`.

**CLOSURE IS SEQUENCED (owner's call, recorded in the status block):**
`re-cook + re-stage + re-bank (G92 wipes Saved) + re-run the A44 hash scan`, **after the current
measurement sequence, never inside one** — closing it **retires `101AFEA4` as the m25 measurement
binary**, so any result still owed against it (today: the whole path-(a) question) must land first.

**G112 was AMENDED IN PLACE, not just supplemented** — its runnable detector is **demoted to
necessary-but-not-sufficient**, and `PRE-DELIVERY-CHECKLIST.md` §1 gained a mandatory read-back box.
**G119** generalises it as the **third instance of one shape**: G92 trusted "it compiled" without
checking it was *staged*; G113 trusted an *exit code* without checking it was *earned*; G118 trusted
the *source config* without checking what the artifact *enforces*. Diagnostic question — **"what would
I observe if the thing I edited never reached the thing under test?"** If the answer is "exactly what
I'm seeing now", it isn't a check.
