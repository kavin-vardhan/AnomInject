# 085-01 — Chat ruling on the m53 delivery-cut plan (NEEDS-DECISION): plan accepted; ND-1..ND-8 as recommended (ND-4 with a yield reading); release hygiene moves to the fix branch first so the m52-only fallback also ships clean

Date: 2026-09-29 ~06:55 IST · Author: chat Claude (owner offline, per his instruction) · Plan `docs/predictions/2026-09-29-m53-dc.md` on `feat/m53-uv-normal-corruption` `0209599`

## Accepted
- **The plan:** 12 IN items mapped, 5 OUT groups deferred, 400 lines.
- **The merge dry run:** 8 files, 13 hunks, all keep-both.
  - **The hidden double destructor in `Anomaly_StuckLowMip.cpp` is a good catch.** Resolve it as planned: one destructor doing both jobs, with a test that both jobs run.
- **The exclusion design:**
  - m52 is live until its trail stops gating (reopened, adopted and carried trails included);
  - m53 is live until revert, plus 2 frames;
  - the id is filtered before the draw, keeping the one-draw contract;
  - the targeted-fire check plus the `ApplyAnomaly` backstop;
  - the capture-registered trail provider;
  - `Auto.Exclude` logging with counters, and an overlap-frames counter that must read 0;
  - EXCL-A / EXCL-B, and the "live until BeginRevert" mutant must fail.
- **The gate table** and the ~2 h 45 min night after 084-09.
- **The schedule reading:** m53's night is Tuesday only if 085-03's dry run is green by 23:00 IST, otherwise Wednesday. The fallback is to cut the delivery from `fix/m52-label-timing`.

## Rulings
- **ND-1: YES, strict.** An empty or translucent slot refuses the whole event. This is the owner's all-or-nothing call as he stated it; yield is reported, not traded.
- **ND-2: YES.**
- **ND-3: YES.** The `ApplyAnomaly` backstop also closes the raw-Apply limitation (082-11 P2-9), so the readme drops that limitation.
- **ND-4: YES**, with a declared bench-only cap raise for the B-M53 MainWorld legs if no target is eligible at 128 MiB.
  - The **delivered default stays 128 MiB** (the owner's product call).
  - The bench **also reports MainWorld yield at the delivered default**, as a separate reading for the owner's Thursday call. If the delivered default yields ~0 on realistic content, the owner must see that number before choosing default-on.
- **ND-5: YES.** The normal-mode "no visible change" rate, per mode on MainWorld, goes in the readme with the m55 drop recipe and feeds the owner's Thursday decision. On the fixture, a no-change event is a FAIL.
- **ND-6: YES** (`-IAIBenchFixture` also opens the bench gate).
- **ND-7: YES** (`mode_invalid:not_in_delivery:<mode>`).
- **ND-8: YES** (G-SYNC-REG on the merged build).

## The unanticipated items
1. **The twins show in delivered builds today.** Release hygiene must also hold on the **m52-only fallback**, so it lands on `fix/m52-label-timing` **first**, in a small brief **084-10**; m53 inherits it through the 085-02 merge. 084-10 covers:
   - the bench twins registered and listed only under `-IAIBench` / `-IAIBenchFixture`;
   - every `IAI.Bench.*` lever gated the same way and compiled out of Shipping (one mechanism);
   - `G-LEVER-AUDIT` (a planted unguarded registration must fail);
   - the dashboard `.env` in the package;
   - the readme host-tools path;
   - the AnomalyBench/fixture exclusion lines;
   - the G354 descriptor check script.

   Any uncommitted AnomDash edits on the home PC are listed, not committed; the owner decides.
2. **m53 carries no TAA flag.** Material swaps (corrupted_texture) proved 0/0 with AA on and no flags in B-REAL, so none is expected. B-M53's AA-on legs decide. If they show a lag, m53 gets the same `taa` reason, as a finding.

## Next (one brief in flight)
1. **084-08b** (the LOD fixture), released now.
2. **084-10** (release hygiene on the fix branch; xhigh).
3. **085-02** (merge fix → m53, the DC source, build, then the Codex source review in the background, collected and not acted on; xhigh).
4. **085-03** (the fixture cook, the B-M53 harness, the dry run).
5. **087-01** (the office kit).
6. **Night:** 084-09, then m53's night if green by 23:00.
