import argparse
import json
from pathlib import Path


MUTATIONS = {
    "class_rule_removed": ("\t\tif (bClassRuleActive)\n", "\t\tif (bClassRuleActive && NumPatterns < 0)\n"),
    "pattern_rule_removed": ("for (int k = 0; Patterns && k < NumPatterns; ++k)", "for (int k = NumPatterns; Patterns && k < NumPatterns; ++k)"),
    "case_sensitive_match": ("return (C >= 'A' && C <= 'Z') ? C + ('a' - 'A') : C;", "return C;"),
}


def main():
    ap = argparse.ArgumentParser(description="Write one mutated copy of AnomalyTerrainRule.h per mutation; the selftest must FAIL on each.")
    ap.add_argument("header")
    ap.add_argument("output")
    args = ap.parse_args()
    source = Path(args.header).read_text(encoding="utf-8").replace("\r\n", "\n")
    created = []
    for name, (old, new) in MUTATIONS.items():
        if source.count(old) != 1:
            raise RuntimeError("mutation anchor changed: " + name)
        path = Path(args.output) / name / "AnomalyTerrainRule.h"
        path.parent.mkdir(parents=True, exist_ok=True)
        if path.exists():
            raise RuntimeError("refusing overwrite: " + str(path))
        path.write_text(source.replace(old, new), encoding="utf-8")
        created.append(str(path))
    print(json.dumps(created, indent=1))


if __name__ == "__main__":
    main()
