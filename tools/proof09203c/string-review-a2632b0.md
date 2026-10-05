Final normal binary 3C135195: 325 new review candidates compared with release 218BA4C7. Source declarations, imported module names, verified source paths, CodeView bytes and decoded Injector DLL EH4 exception metadata resolve 317. Eight exact remaining EXE runs were manually inspected against their surrounding bytes and retained PE address evidence:

- `8M%iB0` overlaps the aligned .text pointer 0x1432ea730 after non-text float bytes.
- `9Y>)F@R!B` overlaps the aligned .text pointer 0x142215240 after repeated non-text float bytes.
- `ltuo .` ends in the aligned .text pointer 0x149e72e20 after packed numeric fields.
- `@08~ab`, `P08~ab`, `@8@HNab`, `@8@Nab` and the run starting with byte 0x60 followed by `8@HVab` are repeated packed metadata with .text RVAs 0x162617e, 0x162614e or 0x1626156. Their offsets, preceding/following bytes and every occurrence are in fragment-address-evidence.json and strings-reviewed-final.json under audits/probe-a2632b0-3C135195.

Only these eight literal runs were added to the reviewed set; no wildcard or semantic-string exemption was introduced. The appended unapproved semantic-string mutant is still classified REVIEW. Two early dependent review invocations ran before strings-reviewed.json was produced and failed FileNotFoundError without producing a verdict; they were rerun after its producer exited successfully.
