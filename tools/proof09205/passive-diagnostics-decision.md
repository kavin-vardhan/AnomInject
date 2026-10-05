# Diagnostic pass must not start a residency measurement

The D3D debug repeat identifies an invalid R32_TYPELESS-to-R8G8B8A8_TYPELESS copy during the inherited census uniformity warmup. It occurs before the new diagnostic aggregation. The exact caller is not established yet; the failure is retained, and Part B must resolve it before any producer proof can pass.

Source inspection also establishes that the inherited census actively enqueues uniformity measurement: TexCorruptTree calls Uniform::KickForActor; TexCorruptUniform creates private Drop-0 VTs, prefetches them, then reports measured asset names asynchronously. That does not meet this brief's passive, numbers-only diagnostic contract.

The opt-in `vt` mode will therefore use the same eligibility/all-reasons evaluator against the existing uniformity cache, without requesting a new measurement. A missing cache entry still returns Pending; no eligibility gate is relaxed or changed. Only diagnostics use this option. Existing census forms, anomaly application, preparation, selection and labels keep their original behavior. The printed census eligibility is a current-cache snapshot; uniformity-pending counts and dependency-unassessed counts are not projected yield.

The harness's legacy warmup census is removed so the new diagnostic is tested as the first census command. The unchanged scene-state and every-frame null+0.05 pixel gates still apply, with the same predeclared count expectations. This is disclosed implementation refinement, not evidence that the inherited copy error has been fixed.
