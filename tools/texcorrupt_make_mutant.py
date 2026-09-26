import sys
from pathlib import Path

FAULTS = [
    ("scratch_class_sharing_off",
     "bClassSeen = Tex[j].M > 1 && Tex[j].W == Tex[i].W && Tex[j].H == Tex[i].H && Tex[j].bSRGB == Tex[i].bSRGB;",
     "bClassSeen = false && Tex[j].M > 1;"),
    ("mip_clamp_removed",
     "return Mip < 0 ? 0 : (Mip > Last ? Last : Mip);",
     "return Mip;"),
    ("pending_retired_a_frame_early",
     "if (Due[i] <= Frame)",
     "if (Due[i] <= Frame + 1)"),
    ("created_bytes_unreserved_not_pending",
     "L.ReleaseToPending(Moved, Frame);",
     "L.Unreserve(Moved);"),
    ("empty_expected_set_reads_held",
     "return EHeld::NoExpectedSet;",
     "return EHeld::Held;"),
    ("usage_first_category_exit",
     "\t\t\tR |= Usage::InstancedStaticMeshes;\n\t\t}",
     "\t\t\tR |= Usage::InstancedStaticMeshes;\n\t\t\treturn R;\n\t\t}"),
    ("chain_limit_reads_clean",
     "return EChainWalk::LimitReached;",
     "return EChainWalk::Clean;"),
    ("collateral_target_not_excluded",
     "return bRegistered && !bIsTarget && ",
     "return bRegistered && "),
]


def main():
    if len(sys.argv) != 3:
        print("usage: texcorrupt_make_mutant.py <TexCorruptPure.h> <out dir>")
        return 2
    src = Path(sys.argv[1]).read_text(encoding="utf-8")
    out_dir = Path(sys.argv[2])
    for name, old, new in FAULTS:
        count = src.count(old)
        if count != 1:
            print(f"FAULT {name}: expected exactly 1 match, found {count}")
            return 2
        src = src.replace(old, new)
        print(f"applied {name}")
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "TexCorruptPure.h").write_text(src, encoding="utf-8", newline="\n")
    print(f"{len(FAULTS)} fault(s) written to {out_dir / 'TexCorruptPure.h'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
