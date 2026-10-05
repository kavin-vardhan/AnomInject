# Private page-table initialization before readiness

Recorded after the failed `prefetch-no-request-01` and before the correction or any new leg.
The original no-request prediction failed on source 46a52d0: the probe reported ready after one
attempt, installed Drop 0, and target images 129..133 exceeded the frozen identity limit. Both
controls stayed within their limits. Preserve the failed result; it is not an accepted refusal.

The bench flag reaches PollPrefetch correctly. UE 5.1.1 `VirtualTextureSpace.cpp:88-105` allocates
and frees virtual address regions without clearing their GPU entries. `VirtualTextureSystem.cpp:
1207-1236` does no pending-load work when there are no queued requests. Page-table values contain
a physical address and mip, not a producer identity. Reading reused entries can therefore satisfy
the old predicate without establishing residency for the new private producer. The observed false
readiness is established; the particular previous owner of those bytes was not traced.

Before the first request/readback, initialize only the new private allocation's page-table region
to zero at its relevant mip levels. Use public IAllocatedVirtualTexture coordinates, dimensions and
producer handles; validate every handle equals the private producer and validate each copy against
the actual RHI texture bounds before writing. The source producer is distinct by the existing
Initialize check. Never clear the whole shared page table or another allocation. If the region
cannot be initialized exactly, refuse through the existing bounded prefetch failure. Request/load
and exact-level GPU readback remain the readiness mechanism. The original binding stays installed.

Prediction: unchanged no-request reader now passes with prefetch_incomplete, no private binding,
no changed target/control image, zero events, and resource/producer drain 0/0. An actual full prefetch
must still pass the original frozen 942x869 identity and the fixed 1920x1080 identity on every edge.
Then repeat the positive PIE/staged matrix, lifetime, can-fails, MainWorld inventory and regressions
on the exact corrected binary. No selection, pixel, mask, E1 or timeout threshold changes. Rebuild
both targets, strict include checks, audits and archives before accepting the new final build.
