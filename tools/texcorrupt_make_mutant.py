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
    ("null_slot_not_measured",
     "return EngineDefault;",
     "return nullptr;"),
    ("unresolved_entry_not_counted",
     "\t\t\t++InOutUnresolved;\n\t\t\treturn ECollEntry::Unresolved;",
     "\t\t\treturn ECollEntry::Unresolved;"),
    ("unresolved_left_out_of_incomplete",
     "return DroppedByCap + UnmeasuredMaterials + Unresolved + UnknownResidency",
     "return DroppedByCap + UnmeasuredMaterials + UnknownResidency"),
    ("rollback_line_judged_at_once",
     "if (ReadingFrame < PostRevertSampleFrame(TerminalFrame))",
     "if (ReadingFrame < TerminalFrame)"),
    ("balance_ignores_pending",
     "return Live == 0 && Pending == 0 ? ELedgerBalance::Balanced",
     "return Live == 0 ? ELedgerBalance::Balanced"),
    ("forward_crc_big_endian_bytes",
     "Out[w * 4 + k] = (unsigned char)((Words[w] >> (8 * k)) & 0xFFu);",
     "Out[w * 4 + k] = (unsigned char)((Words[w] >> (8 * (3 - k))) & 0xFFu);"),
    ("crc_tag_word_dropped",
     "const unsigned Words[3] = { Seed, Ordinal, ScrambleTag };",
     "const unsigned Words[3] = { Seed, Ordinal, 0u };"),
    ("noop_scramble_pair_allowed",
     "if (A == 1 && B == 0)",
     "if (A == 0 && B == -1)"),
    ("v1p_removed_partial_passes",
     "return Qualified < Slots ? EFootprint::Partial : EFootprint::Full;",
     "return EFootprint::Full;"),
    ("mode_family_check_removed",
     "\t\tif (Name->Family != Family)\n\t\t{\n\t\t\treturn EModeArg::WrongFamily;\n\t\t}\n",
     ""),
    ("revert_settling_ends_a_frame_early",
     "return Now < PostRevertFrame;",
     "return Now + 1 < PostRevertFrame;"),
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
