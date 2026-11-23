import argparse
import json
from pathlib import Path


MUTATIONS = {
    "path_reads_as_exact_name": ("return FirstChar == '@' ? ETargetToken::Path", "return FirstChar == '#' ? ETargetToken::Path"),
    "apply_ambiguous_at_one": ("return Kind != ETargetToken::Path && NumActorsNamed > 1;", "return Kind != ETargetToken::Path && NumActorsNamed > 0;"),
    "apply_ambiguity_ignored": ("return Kind != ETargetToken::Path && NumActorsNamed > 1;", "return Kind != ETargetToken::Path && NumActorsNamed > 100000;"),
    "targeted_ambiguity_ignored": ("return Kind != ETargetToken::Path && NumMatches > 1;", "return Kind != ETargetToken::Path && NumMatches > 100000;"),
    "path_token_ambiguous": ("return Kind != ETargetToken::Path && NumMatches > 1;", "return NumMatches > 1;"),
}


def main():
    ap = argparse.ArgumentParser(description="Write one mutated copy of AnomalyTargetPolicy.h per mutation; target_identity_selftest must FAIL on each.")
    ap.add_argument("header")
    ap.add_argument("output")
    args = ap.parse_args()
    source = Path(args.header).read_text(encoding="utf-8").replace("\r\n", "\n")
    created = []
    for name, (old, new) in MUTATIONS.items():
        if source.count(old) != 1:
            raise RuntimeError("mutation anchor changed: " + name)
        path = Path(args.output) / name / "AnomalyTargetPolicy.h"
        path.parent.mkdir(parents=True, exist_ok=True)
        if path.exists():
            raise RuntimeError("refusing overwrite: " + str(path))
        path.write_text(source.replace(old, new), encoding="utf-8")
        created.append(str(path))
    print(json.dumps(created, indent=1))


if __name__ == "__main__":
    main()
