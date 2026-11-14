import argparse
import json
from pathlib import Path


MUTATIONS = {
    "coverage_ignored": ("return (CoveragePct >= CoverageMinPct && bContrast) ? EFrameClass::Positive : EFrameClass::Subvisible;",
                         "return EFrameClass::Positive;"),
    "release_ignores_dither": ("if (T.Mode == EMode::DistanceThreshold && T.LastClass == EFrameClass::Positive && In.bContrast && bAgree",
                               "if (T.Mode == EMode::DistanceThreshold && T.LastClass == EFrameClass::Positive && In.bContrast"),
    "invisible_swap_counted": ("if (In.bOnScreen && In.EngineLod >= 0 && In.EngineLod != T.HeldLod)",
                               "if (In.bOnScreen && In.EngineLod >= 0)"),
    "freeze_ignores_temporal": ("return EngineLod >= 1 && TemporalLod0 == EngineLod && TemporalLod1 == EngineLod;",
                                "return EngineLod >= 1;"),
    "downpop_without_retreat": ("In.bOnScreen && In.bRetreating\n", "In.bOnScreen\n"),
    "strict_engine_compare": ("if (MeshRadius * MeshRadius > RadiusSquared)", "if (MeshRadius * MeshRadius >= RadiusSquared)"),
    "subvisible_holds_open": ("return T.bHeld && T.bHasLast && T.LastClass == EFrameClass::Positive && !T.bCapReached",
                              "return T.bHeld && T.bHasLast && T.LastClass != EFrameClass::Negative && !T.bCapReached"),
    "margin_ignores_first_positive": ("\t\t\t\t&& In.BoundsScreenSize >= T.FirstPositiveScreenSize * (1.0f + Settings.ReleaseMargin))",
                                      "\t\t\t\t)"),
    "entry_frames_off_by_one": ("&& T.PositiveFramesSeen >= Settings.EntryFrames)", "&& T.PositiveFramesSeen > Settings.EntryFrames)"),
    "lookahead_ignores_coverage": ("const float Target = DistanceLookAheadScreenSize(In.ReleaseScreenSize, CoverageSize);",
                                   "const float Target = In.ReleaseScreenSize;"),
    "positive_ignores_contrast": ("return (CoveragePct >= CoverageMinPct && bContrast) ? EFrameClass::Positive",
                                  "return (CoveragePct >= CoverageMinPct) ? EFrameClass::Positive"),
    "arm_clock_no_restart": ("if (bWrongSeen && !Clock.bRestarted)", "if (bWrongSeen && !Clock.bRestarted && TimeoutFrames < 0)"),
    "view_margin_fixed": ("if (In.AngleOutsideFrustumDeg > ViewEntryMarginForRate(Settings, In.YawRateDegPerS, In.FrameSeconds))",
                          "if (In.AngleOutsideFrustumDeg > Settings.ViewEntryMarginDeg)"),
}


def main():
    ap = argparse.ArgumentParser(description="Write one mutated copy of AnomalyLodTriggerRule.h per mutation; the selftest must FAIL on each.")
    ap.add_argument("header")
    ap.add_argument("output")
    args = ap.parse_args()
    source = Path(args.header).read_text(encoding="utf-8").replace("\r\n", "\n")
    created = []
    for name, (old, new) in MUTATIONS.items():
        if source.count(old) != 1:
            raise RuntimeError("mutation anchor changed: " + name)
        path = Path(args.output) / name / "AnomalyLodTriggerRule.h"
        path.parent.mkdir(parents=True, exist_ok=True)
        if path.exists():
            raise RuntimeError("refusing overwrite: " + str(path))
        path.write_text(source.replace(old, new), encoding="utf-8")
        created.append(str(path))
    print(json.dumps(created, indent=1))


if __name__ == "__main__":
    main()
