import argparse
import json
from pathlib import Path


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("header")
    ap.add_argument("output")
    args = ap.parse_args()
    source = Path(args.header).read_text(encoding="utf-8")
    mutations = {
        "shared_hold": ("(bShared ? ERoute::Proxy : ERoute::Hold)", "(bShared ? ERoute::Hold : ERoute::Hold)"),
        "pure_proxy": ("(bShared ? ERoute::Proxy : ERoute::Hold)", "(bShared ? ERoute::Proxy : ERoute::Proxy)"),
        "depth_off_one": ("Resident - Target : 0", "Resident - Target + 1 : 0"),
        "ignore_floor": ("Target < Floor ? Floor", "Target < Floor ? Target"),
        "proxy_state": ("bProxy ? AnomalyLabelSync::EAnnotationPolicy::FireWindow", "bProxy ? AnomalyLabelSync::EAnnotationPolicy::AnomalyState"),
        "hold_firewindow": (": HoldPolicy;", ": AnomalyLabelSync::EAnnotationPolicy::FireWindow;"),
    }
    created = []
    for name, (old, new) in mutations.items():
        if source.count(old) != 1:
            raise RuntimeError("mutation anchor changed: " + name)
        path = Path(args.output) / name / "AnomalyProxyBlurPolicy.h"
        path.parent.mkdir(parents=True, exist_ok=True)
        if path.exists():
            raise RuntimeError("refusing overwrite: " + str(path))
        path.write_text(source.replace(old, new), encoding="utf-8")
        created.append(str(path))
    print(json.dumps(created, indent=1))


if __name__ == "__main__":
    main()
