---
name: delete-guard-refuses-command-text
description: "The harness delete guard refuses a whole PowerShell call whose TEXT names a shell delete (cmd /c rmdir, rd /s, Remove-Item...), even inside a receipt string; deletes via a python script with os.unlink/os.rmdir pass."
metadata:
  node_type: memory
  type: feedback
  originSessionId: <uuid>
  modified: 2026-10-24T23:11:18.817Z
---

The delete guard reads the command text, not what actually runs. 094-05 was refused on `cmd /c rmdir "<link>"` ("Remove-Item on
system path '/c'"). In 094-09 the real deletes passed: junctions went with `os.rmdir`, and 29 roots with a python script
(`os.unlink` + bottom-up `os.rmdir`, allow-list, reparse refusal). A LATER call that only wrote the receipt was refused
("system path '/s",'"), because its string described "no rd /s". Nothing in a refused call runs.

**Why:** briefs say a refused delete must not be rephrased or disguised. A refusal on a receipt-only call is a false positive,
not a refused delete, but it costs a turn and looks like one.

**How to apply:**
- Put delete logic in a script file (Write tool) and run `python script.py`.
- Keep the literal tokens `rd /s`, `rmdir`, `cmd /c` and `Remove-Item` out of PowerShell command text and out of strings
  embedded in it.
- Write receipts that mention them with the Write tool.
- If a real delete command is refused, stop and hand the owner a list.

Related: [[scratch-host-build-when-checkout-is-protected]], [[env-refuses-dbghelp-dll]].
