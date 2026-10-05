# Prime the private root through the renderer before full readiness

Recorded before the next correction and its legs. Source a7d4874 / 0F79605D passed the corrected
no-request can-fail (120 attempts, 341 pages, no bind, unchanged pixels), the original frozen
identity and native identity. Its ten-fire positive attempt then produced zero events: uniform
prewarming failed with prefetch_incomplete within the initial measurement ticks and cached the
textures as unmeasurable. The positive checker correctly failed insufficient events and E1.

The manual GPU initialization assumes backing exists before the renderer's pending-load call and
can also diverge from the renderer's CPU mapping state. Remove those direct page-table writes.
Use a two-phase public-API protocol instead: request/load the private allocation's coarsest level,
then read its whole region and require valid mappings at that coarsest level; only after that
observation request/load all required levels and require each mapped level to be exact. The first
phase makes allocation/root mapping go through AllocateResources and SubmitRequests, including
TryMapLockedTiles and ApplyUpdates. The source producer is never requested. Pending/saturated
work remains bounded by the existing 120-attempt / 10-second limits.

The existing no_request bench argument continues to suppress ALL explicit tile requests in both
phases. Automatic engine root residency may occur, but missing fine levels must never be accepted
as full readiness. Require its unchanged reader to pass with positive attempts, 341 pages, no bind,
unchanged target/controls and 0/0 drain. Then repeat original/native identity and the complete
positive/lifetime/can-fail/regression matrix. No proof threshold, selection rule or label/capture
behavior changes. Keep all failed runs, both implementations and their binary archives.
