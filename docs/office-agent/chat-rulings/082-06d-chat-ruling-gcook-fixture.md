# 082-06d — Chat ruling on 082-06 (NEEDS-DECISION): MainWorld cook-identity row amended to the control-pair form; fixture slot fix and re-cook authorised

Date: 2026-09-27 (IST) · Author: chat Claude (orchestrator) · Branch `e070c6c` · CaptureBench `8e53dea`

## 1. The `MainWorld` G-COOK row: amended (dated, pre-staging)
Two cooks from identical inputs produce different `MainWorld` chunks (`0B2F9571`, `A9D83F53`, `2B46C64E`; same size). Exact identity is therefore **unsatisfiable**. The row asked for a determinism the engine doesn't provide. The row's intent is that m53's additions must not change `MainWorld`'s cooked content. M1 and M2 already prove every `MainWorld` source file is byte-identical.

**New rule (the G-S3a-1 control-pair form):**
- The region allowed to vary is the union of the differences between **≥ 2 identical-input cooks**.
- The new cook's differences from the archived pre-m53 container must fall **entirely inside that region**.
- The cooked `.umap` header must stay **byte-identical**.

`CB_GateLevel` keeps **exact** identity, as it has today. Record this as a dated plan amendment. Today's reading (696 of 697 differing words inside the region, header identical) is consistent with the rule, but it must be re-read on the re-cook.

## 2. Fixture fix + re-cook: authorised
- **Fix the tool.**
  - Set static-mesh slots with `SetMaterial` on the asset.
  - Build real material entries for the skeletal duplicate.
  - The verify pass must **read the slots back from disk** after saving.
  - The `slot_empty`, asset-slot-MID, `nanite_override` and `default_material_path` producers must each be proven by that read-back.
- **Re-run the tool** only for the affected packages, then take **M1′**. Anything outside the allowed paths is still a stop.
- **Cook location.** The scratch host whose `Content` is linked to the real project is acceptable for **commandlet** runs, provided:
  - byte manifests (M0 → M1′ → M2′) bracket every commandlet and cook run;
  - nobody makes an interactive editor save on that host.

  Record this as a runbook note.
- **Re-cook** (the pre-m53 archive from 082-06 stays valid).
  - Map gate.
  - G-COOK under the amended `MainWorld` row. If the variance region needs another identical-input cook of the new inputs to be well defined, run it.
  - `CB_GateLevel` exact.
  - Every fixture package must be present in the container.
- **Only if G-COOK passes:** stage the exe and container, record the provenance, run A44 on the staged exe, then take **M2′**.
