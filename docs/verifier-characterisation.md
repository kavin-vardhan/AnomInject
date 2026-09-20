# Verifier characterisation — 079-13

This is a measured limitation record, not label approval. Recovery compares an observed offset with an injected label perturbation; it does not establish cause or correctness of the original label. Regional motion remains a reading. Values above the historical 0.42 marker add a caveat. Threshold, region, baseline and input-coverage refusals remain.

Each original run edge has 13 perturbation/variant keys. Every planned key remains in either the scored or unscored ledger. READING/UNASSESSABLE runs contribute zero scored cells. Rates use each session's planned denominator, not only the observed subset.

## Per-session readings

| Session | Planned | Scored | Unscored | recovered | wrong | no-transition | unassessable | unsupported | Positive original edges |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| A2L_LEGA | 208 | 70 | 138 | 46 | 16 | 0 | 8 | 0 | 6 |
| LYRA_SMOKE_01 | 208 | 118 | 90 | 73 | 24 | 0 | 21 | 0 | 9 |
| A1L_LEGA | 156 | 126 | 30 | 99 | 15 | 2 | 10 | 0 | 12 |
| TOTAL | 572 | 314 | 258 | 218 | 55 | 2 | 39 | 0 | 27 |

## Every wrong recovery — known limitations

Expected/observed values are offsets relative to the shifted label. All window peaks are listed; [used earlier] marks peaks excluded before this edge's choice. Nearest candidates win; equidistant candidates prefer larger d, then earlier frame. Thus a nearer unrelated peak can still beat a stronger intended transition. One-sided prominence admits plateau edges and adjacent changes. Different regions may yield out-of-order assignments, which are disclosed.

| Cell | Label | Selected | Expected | Observed | m_edge | Competing peaks |
| --- | --- | --- | --- | --- | --- | --- |
| A2L_LEGA event3 run0 onset onset+1/annotation | 40 | 40 | -1 | 0 | 0.0011036926011718218 | 36 d=0.19933600, 37 d=0.15221149, 39 d=0.99944259, 40 d=0.03145988, 43 d=0.16346337, 44 d=0.38385509 |
| A2L_LEGA event4 run0 onset onset+1/annotation | 52 | 52 | -1 | 0 | 5.820721769499418e-05 | 48 d=0.12063446, 51 d=0.99985448, 52 d=0.09403376, 53 d=0.00681024, 56 d=0.09534897 |
| A2L_LEGA event5 run0 onset onset+1/annotation | 76 | 76 | -1 | 0 | 5.820721769499418e-05 | 72 d=0.03009313, 75 d=1.00000000, 76 d=0.07001497 |
| A2L_LEGA event3 run0 onset onset+1/coherent | 40 | 40 | -1 | 0 | 0.0011036926011718218 | 36 d=0.19933600, 37 d=0.15221149, 39 d=0.99944259, 40 d=0.03145988, 43 d=0.16346337, 44 d=0.38385509 |
| A2L_LEGA event4 run0 onset onset+1/coherent | 52 | 52 | -1 | 0 | 5.820721769499418e-05 | 48 d=0.12063446, 51 d=0.99985448, 52 d=0.09403376, 53 d=0.00681024, 56 d=0.09534897 |
| A2L_LEGA event5 run0 onset onset+1/coherent | 76 | 76 | -1 | 0 | 5.820721769499418e-05 | 72 d=0.03009313, 75 d=1.00000000, 76 d=0.07001497 |
| A2L_LEGA event3 run0 end end+1/coherent | 48 | 48 | -1 | 0 | 0.0017577326611254939 | 44 d=0.38385509, 45 d=0.68355780, 47 d=0.99995912, 48 d=0.62592996, 52 d=0.73493664 |
| A2L_LEGA event4 run0 end end+1/coherent | 60 | 60 | -1 | 0 | 5.820721769499418e-05 | 56 d=0.09534897, 59 d=1.00000000, 60 d=0.12593132, 62 d=0.00497672 |
| A2L_LEGA event5 run0 end end+1/coherent | 84 | 84 | -1 | 0 | 0.0 | 81 d=0.04412661, 83 d=0.99994179, 84 d=0.02348661, 85 d=0.00713038, 88 d=0.02328289 |
| A2L_LEGA event5 run0 onset both+1/annotation | 76 | 76 | -1 | 0 | 5.820721769499418e-05 | 72 d=0.03009313, 75 d=1.00000000, 76 d=0.07001497 |
| A2L_LEGA event3 run0 onset both+1/coherent | 40 | 40 | -1 | 0 | 0.0011036926011718218 | 36 d=0.19713557, 37 d=0.15061596, 39 d=0.99984910, 40 d=0.02775263, 43 d=0.16053115, 44 d=0.36634362 |
| A2L_LEGA event3 run0 end both+1/coherent | 48 | 48 | -1 | 0 | 0.0011036926011718218 | 44 d=0.36634362, 45 d=0.64734447, 47 d=0.98028483, 48 d=0.62592996, 52 d=0.73493664 |
| A2L_LEGA event4 run0 onset both+1/coherent | 52 | 52 | -1 | 0 | 5.820721769499418e-05 | 48 d=0.12063446, 51 d=0.99985448, 52 d=0.09403376, 53 d=0.00681024, 56 d=0.05011641 |
| A2L_LEGA event4 run0 end both+1/coherent | 60 | 60 | -1 | 0 | 5.820721769499418e-05 | 56 d=0.05011641, 59 d=1.00000000, 60 d=0.12593132, 62 d=0.00497672 |
| A2L_LEGA event5 run0 onset both+1/coherent | 76 | 76 | -1 | 0 | 0.0 | 75 d=1.00000000, 76 d=0.05026193 |
| A2L_LEGA event5 run0 end both+1/coherent | 84 | 84 | -1 | 0 | 0.0 | 83 d=0.99994456, 84 d=0.02348661, 85 d=0.00713038, 88 d=0.02328289 |
| LYRA_SMOKE_01 event3 run0 end both+0/annotation | 59 | 60 | 0 | 1 | 0.21171586715867158 | 55 d=0.77722714, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event0 run1 onset onset+1/annotation | 22 | 23 | -1 | 1 | 0.002306219131135974 | 18 d=0.65651048 [used earlier], 19 d=0.02811813, 21 d=0.63133631, 23 d=0.63662937 |
| LYRA_SMOKE_01 event0 run1 end onset+1/annotation | 23 | 22 | 0 | -1 | 0.002306219131135974 | 22 d=0.05944479, 23 d=0.63662937 [used earlier], 24 d=0.05334140 |
| LYRA_SMOKE_01 event3 run0 end onset+1/annotation | 59 | 60 | 0 | 1 | 0.20285977859778598 | 55 d=0.77722714, 60 d=0.67776753 |
| LYRA_SMOKE_01 event0 run1 onset onset+1/coherent | 22 | 23 | -1 | 1 | 0.002306219131135974 | 18 d=0.65651048 [used earlier], 19 d=0.02811813, 21 d=0.63133631, 23 d=0.63662937 |
| LYRA_SMOKE_01 event0 run1 end onset+1/coherent | 23 | 22 | 0 | -1 | 0.002306219131135974 | 22 d=0.05944479, 23 d=0.63662937 [used earlier], 24 d=0.05334140 |
| LYRA_SMOKE_01 event3 run0 end onset+1/coherent | 59 | 60 | 0 | 1 | 0.20285977859778598 | 55 d=0.77722714, 60 d=0.67776753 |
| LYRA_SMOKE_01 event3 run0 end onset-1/annotation | 59 | 60 | 0 | 1 | 0.21220018450184502 | 55 d=0.77722714, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event3 run0 end onset-1/coherent | 59 | 60 | 0 | 1 | 0.21220018450184502 | 55 d=0.77722714, 60 d=0.67776753, 63 d=1.00000000 |
| LYRA_SMOKE_01 event0 run0 end end+1/coherent | 19 | 19 | -1 | 0 | 0.0035830001170123913 | 16 d=0.84143207 [used earlier], 18 d=0.65735123, 19 d=0.02812379, 21 d=0.63212091 |
| LYRA_SMOKE_01 event0 run1 end end+1/coherent | 24 | 24 | -1 | 0 | 0.0035717018132326115 | 21 d=0.63292408 [used earlier], 23 d=0.63662937, 24 d=0.05334140 |
| LYRA_SMOKE_01 event1 run0 end end+1/coherent | 36 | 36 | -1 | 0 | 0.06118311848843369 | 35 d=0.99991193, 36 d=0.15438601, 40 d=0.43068117 |
| LYRA_SMOKE_01 event3 run0 end end+1/coherent | 60 | 60 | -1 | 0 | 0.20728782287822878 | 56 d=0.90002575, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event4 run0 end end+1/coherent | 72 | 72 | -1 | 0 | 0.018656308983418347 | 71 d=1.00000000, 72 d=0.59385091, 76 d=0.08230770 |
| LYRA_SMOKE_01 event5 run0 onset both+1/annotation | 88 | 88 | -1 | 0 | 0.027754973515817412 | 87 d=0.99957443, 88 d=0.07046544 |
| LYRA_SMOKE_01 event0 run0 end both+1/coherent | 19 | 19 | -1 | 0 | 0.0029454843304268534 | 18 d=0.65695776, 19 d=0.02812379, 21 d=0.63212091, 22 d=0.05960756 |
| LYRA_SMOKE_01 event0 run1 onset both+1/coherent | 22 | 23 | -1 | 1 | 0.0029395065655691164 | 19 d=0.02806147 [used earlier], 21 d=0.63292408, 23 d=0.63420354, 24 d=0.05334140 |
| LYRA_SMOKE_01 event0 run1 end both+1/coherent | 24 | 24 | -1 | 0 | 0.0029395065655691164 | 22 d=0.05960756, 23 d=0.63420354 [used earlier], 24 d=0.05334140 |
| LYRA_SMOKE_01 event1 run0 end both+1/coherent | 36 | 36 | -1 | 0 | 0.05909941726999823 | 35 d=0.99913673, 36 d=0.15438601, 40 d=0.43068117 |
| LYRA_SMOKE_01 event3 run0 onset both+1/coherent | 52 | 52 | -1 | 0 | 0.20285977859778598 | 49 d=0.56099166, 52 d=0.89425186, 56 d=0.61472156 |
| LYRA_SMOKE_01 event3 run0 end both+1/coherent | 60 | 60 | -1 | 0 | 0.20285977859778598 | 56 d=0.61472156, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event4 run0 end both+1/coherent | 72 | 72 | -1 | 0 | 0.018617311682031602 | 71 d=1.00000000, 72 d=0.59385091, 76 d=0.08230770 |
| LYRA_SMOKE_01 event3 run0 onset both-1/coherent | 50 | 48 | 1 | -2 | 0.21268450184501844 | 48 d=0.48632441, 54 d=0.73434014 |
| LYRA_SMOKE_01 event3 run0 end both-1/coherent | 58 | 60 | 1 | 2 | 0.21268450184501844 | 54 d=0.73434014, 60 d=0.67776753 |
| A1L_LEGA event0 run0 end both+0/annotation | 6 | 8 | 0 | 2 | 0.0 | 4 d=0.60673932 [used earlier], 8 d=1.00000000 |
| A1L_LEGA event0 run0 onset onset+1/annotation | 5 | 6 | -1 | 1 | 0.001156737998843262 | 4 d=0.54235166, 6 d=0.99624060 |
| A1L_LEGA event0 run0 end onset+1/annotation | 6 | 5 | 0 | -1 | 0.001156737998843262 | 5 d=0.68165487, 8 d=1.00000000 |
| A1L_LEGA event1 run0 onset onset+1/annotation | 16 | 16 | -1 | 0 | 0.1035309750759843 | 15 d=0.99999473, 16 d=0.50537821 |
| A1L_LEGA event0 run0 onset onset+1/coherent | 5 | 6 | -1 | 1 | 0.001156737998843262 | 4 d=0.54235166, 6 d=0.99624060 |
| A1L_LEGA event0 run0 end onset+1/coherent | 6 | 5 | 0 | -1 | 0.001156737998843262 | 5 d=0.68165487, 8 d=1.00000000 |
| A1L_LEGA event1 run0 onset onset+1/coherent | 16 | 16 | -1 | 0 | 0.1035309750759843 | 15 d=0.99999473, 16 d=0.50537821 |
| A1L_LEGA event0 run0 end onset-1/coherent | 6 | 8 | 0 | 2 | 0.0 | 4 d=0.60673932 [used earlier], 8 d=1.00000000 |
| A1L_LEGA event0 run0 end end+1/coherent | 7 | 8 | -1 | 1 | 0.0 | 4 d=0.60673932 [used earlier], 8 d=1.00000000 |
| A1L_LEGA event2 run1 end end+1/coherent | 36 | 36 | -1 | 0 | 0.1199678923629692 | 33 d=0.99966365 [used earlier], 35 d=0.99931198, 36 d=0.52524425, 39 d=0.88644905 |
| A1L_LEGA event0 run0 end end-1/annotation | 5 | 8 | 1 | 3 | 0.12505850103630406 | 8 d=1.00000000 |
| A1L_LEGA event0 run0 end end-1/coherent | 5 | 8 | 1 | 3 | 0.12505850103630406 | 8 d=1.00000000 |
| A1L_LEGA event1 run0 onset both+1/coherent | 16 | 16 | -1 | 0 | 0.12561668944495766 | 15 d=0.99999473, 16 d=0.50537821 |
| A1L_LEGA event2 run1 end both+1/coherent | 36 | 36 | -1 | 0 | 0.1199678923629692 | 35 d=0.99866507, 36 d=0.52524425, 39 d=0.88644905 |
| A1L_LEGA event3 run0 onset both+1/coherent | 52 | 52 | -1 | 0 | 0.02057720812625338 | 51 d=1.00000000, 52 d=0.84183451, 55 d=0.84510466, 56 d=0.41943577 |

## Previously wrong cells after R8/R9

| Cell | Previous expected / observed | Current status | Expected | Observed | m_edge | Competing peaks |
| --- | --- | --- | --- | --- | --- | --- |
| A2L_LEGA event3 run0 end end-1/annotation | 1 / -1 | recovered | 1 | 1 | 0.0016533591418662794 | 43 d=0.16346337, 44 d=0.38385509, 45 d=0.68355780, 47 d=0.98028483, 48 d=0.60327446, 50 d=0.44127215 |
| A2L_LEGA event5 run0 end end-1/annotation | 1 / -1 | recovered | 1 | 1 | 0.0 | 81 d=0.04412661, 83 d=0.99994456 |
| A2L_LEGA event3 run0 end end-1/coherent | 1 / -1 | recovered | 1 | 1 | 0.0016533591418662794 | 43 d=0.16346337, 44 d=0.38385509, 45 d=0.68355780, 47 d=0.98028483, 48 d=0.60327446, 50 d=0.44127215 |
| A2L_LEGA event5 run0 end end-1/coherent | 1 / -1 | recovered | 1 | 1 | 0.0 | 81 d=0.04412661, 83 d=0.99994456 |
| A2L_LEGA event3 run0 end both-1/annotation | 1 / -1 | recovered | 1 | 1 | 0.0016533591418662794 | 43 d=0.16346337, 44 d=0.38385509, 45 d=0.68355780, 47 d=0.98028483, 48 d=0.60327446, 50 d=0.44127215 |
| A2L_LEGA event5 run0 end both-1/annotation | 1 / -1 | recovered | 1 | 1 | 0.0 | 81 d=0.04412661, 83 d=0.99994456 |
| A2L_LEGA event3 run0 end both-1/coherent | 1 / -1 | recovered | 1 | 1 | 0.0016214743153018121 | 43 d=0.16700627, 44 d=0.35910410, 45 d=0.66704713, 47 d=0.99995912, 48 d=0.62592996, 50 d=0.46389154 |
| A2L_LEGA event5 run0 end both-1/coherent | 1 / -1 | recovered | 1 | 1 | 1.4551804423748545e-05 | 81 d=0.04412661, 83 d=0.99994179, 84 d=0.02348661, 85 d=0.00713038 |
| LYRA_SMOKE_01 event3 run0 end both+0/annotation | 0 / 4 | wrong | 0 | 1 | 0.21171586715867158 | 55 d=0.77722714, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event3 run0 end onset-1/annotation | 0 / 4 | wrong | 0 | 1 | 0.21220018450184502 | 55 d=0.77722714, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event3 run0 end onset-1/coherent | 0 / 4 | wrong | 0 | 1 | 0.21220018450184502 | 55 d=0.77722714, 60 d=0.67776753, 63 d=1.00000000 |
| LYRA_SMOKE_01 event3 run0 end end+1/coherent | -1 / 3 | wrong | -1 | 0 | 0.20728782287822878 | 56 d=0.90002575, 60 d=0.67776753, 63 d=0.87669582 |
| LYRA_SMOKE_01 event3 run0 end both+1/coherent | -1 / 3 | wrong | -1 | 0 | 0.20285977859778598 | 56 d=0.61472156, 60 d=0.67776753, 63 d=0.87669582 |
| A1L_LEGA event3 run0 onset both+0/annotation | 0 / 4 | recovered | 0 | 0 | 0.019164213027532413 | 51 d=1.00000000, 55 d=0.98550725 |
| A1L_LEGA event3 run0 onset onset+1/annotation | -1 / 3 | recovered | -1 | -1 | 0.03192006106446464 | 51 d=0.65483703, 55 d=0.98550725, 56 d=0.51220030 |
| A1L_LEGA event3 run0 onset onset+1/coherent | -1 / 3 | recovered | -1 | -1 | 0.03192006106446464 | 51 d=0.65483703, 55 d=0.98550725, 56 d=0.51220030 |
| A1L_LEGA event3 run0 onset end+1/annotation | 0 / 4 | recovered | 0 | 0 | 0.019661696747754816 | 51 d=1.00000000, 55 d=0.98550725 |
| A1L_LEGA event3 run0 onset end+1/coherent | 0 / 4 | recovered | 0 | 0 | 0.021075468774213544 | 51 d=1.00000000, 55 d=0.98550725 |
| A1L_LEGA event3 run0 onset end-1/annotation | 0 / 4 | recovered | 0 | 0 | 0.019891577887278685 | 51 d=1.00000000, 55 d=0.98550725 |
| A1L_LEGA event3 run0 onset end-1/coherent | 0 / 4 | recovered | 0 | 0 | 0.019891577887278685 | 51 d=1.00000000, 55 d=0.98550725 |
| A1L_LEGA event3 run0 onset both+1/annotation | -1 / 3 | recovered | -1 | -1 | 0.03591006869752272 | 51 d=0.65483703, 55 d=0.98550725, 56 d=0.51220030 |
| A1L_LEGA event3 run0 onset both+1/coherent | -1 / 3 | wrong | -1 | 0 | 0.02057720812625338 | 51 d=1.00000000, 52 d=0.84183451, 55 d=0.84510466, 56 d=0.41943577 |

## Coverage reasons

| Unscored reason | Keys |
| --- | --- |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (14 unavailable) | 20 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (16 unavailable) | 16 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (17 unavailable) | 16 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (13 unavailable) | 11 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (16 unavailable) | 9 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (15 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 6 | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 30 | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 35 | 8 |
| UNASSESSABLE: neither edge observed; baseline: only 2 valid clean pairs, need 3 (25 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (22 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; baseline: only 2 valid clean pairs, need 3 (24 unavailable) | 8 |
| UNASSESSABLE: neither edge observed; mask missing at frame 53 | 7 |
| UNASSESSABLE: neither edge observed; mask missing at frame 23 | 6 |
| UNASSESSABLE: neither edge observed; mask missing at frame 47 | 6 |
| UNASSESSABLE: neither edge observed; baseline: only 2 valid clean pairs, need 3 (15 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 0; boundary mask unavailable: mask missing at frame 3 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 2; boundary mask unavailable: mask missing at frame 3 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 23; boundary mask unavailable: mask missing at frame 27 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 26; boundary mask unavailable: mask missing at frame 27 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 30; boundary mask unavailable: mask missing at frame 32 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 31; boundary mask unavailable: mask missing at frame 32 | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (13 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 11 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 59 | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 2 valid clean pairs, need 3 (16 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (23 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (21 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 18 | 4 |
| UNASSESSABLE: neither edge observed; mask missing at frame 42 | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (24 unavailable) | 4 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (14 unavailable) | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 6; boundary mask unavailable: mask missing at frame 8 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 7; boundary mask unavailable: mask missing at frame 8 | 2 |
| UNASSESSABLE: neither edge observed; mask id 215 not present at frame 9; boundary mask unavailable: mask missing at frame 14 | 2 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (12 unavailable) | 2 |
| UNASSESSABLE: neither edge observed; baseline: only 1 valid clean pairs, need 3 (15 unavailable) | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 10; boundary mask unavailable: mask missing at frame 15 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 14; boundary mask unavailable: mask missing at frame 15 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 18; boundary mask unavailable: mask missing at frame 20 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 19; boundary mask unavailable: mask missing at frame 20 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 23; boundary mask unavailable: mask missing at frame 26 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 35; boundary mask unavailable: mask missing at frame 39 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 38; boundary mask unavailable: mask missing at frame 39 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 42; boundary mask unavailable: mask missing at frame 44 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 43; boundary mask unavailable: mask missing at frame 44 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 81; boundary mask unavailable: mask missing at frame 86 | 2 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (23 unavailable) | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 71 | 2 |
| UNASSESSABLE: neither edge observed; mask missing at frame 69; boundary mask unavailable: mask missing at frame 74 | 2 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=2.3006 >= 1.0 | 2 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (22 unavailable) | 1 |
| UNASSESSABLE: neither edge observed; edge truncated by the session boundary | 1 |
| UNASSESSABLE: neither edge observed; baseline: only 0 valid clean pairs, need 3 (25 unavailable) | 1 |
| UNASSESSABLE: neither edge observed; mask missing at frame 47; boundary mask unavailable: mask missing at frame 50 | 1 |
| UNASSESSABLE: neither edge observed; mask missing at frame 85; boundary mask unavailable: mask missing at frame 86 | 1 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=1.9478 >= 1.0 | 1 |
| UNASSESSABLE: neither edge observed; threshold unsatisfiable: tau=2.5772 >= 1.0 | 1 |

## Provenance and disposition

Verifier SHA256: `badd0e3e867814d1de702f4259d9a23f94d8ed3738b72f360c91740cc38ed8d0`. Scorer SHA256: `159e6944534cc348bb6a8711d8886766c4b541ce87cea10af8d458c3c17d0f2f`. Immutable planned keys: 572. The JSON ledger contains every scored and unscored key, candidate strengths and tie/ordering diagnostics.

**Chat review REQUIRED** for completed evidence and independent Code review before merge. Client/Section G, campaign and release holds remain; this document does not release them.
