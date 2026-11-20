# Owner's answer to ruling 2 of 090-10x (product decision) — 2026-10-01 ~21:00 IST

**Answer: B.**

When only some material parts (slots/components) of an object can be corrupted, **corrupt just those parts**, with the **label and mask covering only the corrupted parts**. This supersedes the owner's 2026-09-28 "all-or-nothing per object" delivery-cut rule for UV/normal corruption.

Implementation constraints (chat):
- Atomicity is kept **within** the selected coherent map set (all required bindings of a corrupted slot, transactional rollback, exact restoration).
- The mask/label for the event covers **exactly the corrupted components/slots**, never the whole object when parts were skipped. If the mask granularity can only be per component, a slot-level partial on a multi-slot component must either corrupt all of that component's eligible-and-required slots or skip that component — never mask unchanged components.
- Enumerate the changed components/slots/bindings in the event record; the readme states the new meaning plainly (field semantics + §8.8).
- Tests both ways; in-engine pixel proof that the masked area matches the changed pixels' components on a multi-slot object.
