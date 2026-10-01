import argparse
import os
import sys

CAP = "Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp"
LW = "Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp"

COMPLETION = [
    (CAP, "void UAnomalyCaptureSubsystem::ProcessCompletedFrames("),
    (CAP, "void UAnomalyCaptureSubsystem::FillAnnotationInputs("),
    (CAP, "void UAnomalyCaptureSubsystem::AccumulateFrameEvents("),
    (CAP, "void UAnomalyCaptureSubsystem::ApplyRenderTruthToSnapshot("),
    (CAP, "void UAnomalyCaptureSubsystem::ResolveDetachedTransitionCandidates("),
    (CAP, "void UAnomalyCaptureSubsystem::ComputeRenderMembership("),
    (LW, "FString BuildFrameLabelRecord("),
    (LW, "FString BuildLabelRecordForSnapshot("),
    (LW, "bool ProjectFrozenFireBox("),
    (LW, "bool ProjectSnapshotFireBox("),
    (LW, "bool IsFireInAnnotation("),
    (LW, "bool IsSnapshotEntryLabelled("),
    (LW, "int32 MarkInterruptedEffects("),
    (LW, "int32 MarkNaniteUnmaskable("),
    (LW, "int32 MarkCaptureUnpaired("),
]

FORBIDDEN = ("TargetActor.Get(", "TargetActor->", "ProjectActorBoundsToScreenRect(", "GetActorRenderableBounds(",
             "GetActorLocation(", "ProjectFireBox(", "FreezeFireGeometry(", "FreezeSnapshotGeometry(",
             "FreezeSampleGeometry(", "FreezeEventAnchor(", "ResolveNodeIdentity(", "EvaluateSelectionProvenance(",
             "ResolveCameraPath(", "GetPathName(")

ALLOWED = (
    "Async->MaskMeasure.FindOrAddRecord(F.Id, F.Target, F.StartFrame, const_cast<AActor*>(F.TargetActor.Get()));",
)

REQUIRED = [
    (CAP, "void UAnomalyCaptureSubsystem::SampleDeferredActiveState(", ("StepHideTransitions(*Snap);\n\tFreezeSampleGeometry(*Snap);",),
     "the tick-end sample freezes the geometry right after the hide transitions"),
    (CAP, "void UAnomalyCaptureSubsystem::FreezeSampleGeometry(", ("AnomalyLabel::FreezeSnapshotGeometry(Snap);", "FreezeEventAnchor(F);"),
     "the sample freezes every fire's box, transition and candidate boxes, position and event anchor"),
    (CAP, "void UAnomalyCaptureSubsystem::FillAnnotationInputs(", ("AnomalyLabel::ProjectSnapshotFireBox(Snap, i, Min, Max)",),
     "on-screen membership is projected from the frozen box"),
    (CAP, "void UAnomalyCaptureSubsystem::AccumulateFrameEvents(", ("AnomalyLabel::ProjectFrozenFireBox(", "Async->FrozenAnchors.Find(FrozenAnchorKey(F))"),
     "coverage and the event anchor come from the sample"),
    (LW, "FString BuildFrameLabelRecord(", ("AnomalyLabel::ProjectFrozenFireBox(Geometry, View, Min, Max)", "&& !bCaptureUnpaired",
                                         "TEXT(\"capture_unpaired\")"),
     "the record's box is the frozen box, and an unpaired frame is never labelled and carries the root flag"),
    (CAP, "void UAnomalyCaptureSubsystem::CaptureCurrentFrame(", ("SyncFrame.bCaptureUnpaired = true;", "AnomalyLabel::MarkCaptureUnpaired(SyncFrame);",
                                                                 "AnomalyLabel::FreezeSnapshotGeometry(SyncFrame);"),
     "every sync-path frame is flagged capture_unpaired and its entries marked"),
]

ABSENT = [
    (CAP, "void UAnomalyCaptureSubsystem::CaptureCurrentFrame(", ("AccumulateFrameEvents(",),
     "a sync-path frame never reaches annotation.json"),
    (CAP, "void UAnomalyCaptureSubsystem::FinalizeArmedLabel(", ("GetActorLocation(",),
     "the fire position is no longer read at the end of the capture tick, before the sample"),
]


def _norm(text):
    return text.replace("\r\n", "\n")


def body_of(text, signature):
    i = text.find(signature)
    if i < 0 or text.find(signature, i + 1) >= 0:
        return None
    j = text.find("{", i)
    if j < 0:
        return None
    depth = 0
    k = j
    n = len(text)
    while k < n:
        c = text[k]
        if c == '"' or c == "'":
            q = c
            k += 1
            while k < n and text[k] != q:
                if text[k] == "\\":
                    k += 1
                k += 1
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return text[j:k + 1]
        k += 1
    return None


def check(read):
    out = []
    ok = True
    cache = {}

    def src(rel):
        if rel not in cache:
            cache[rel] = _norm(read(rel))
        return cache[rel]

    for rel, sig in COMPLETION:
        b = body_of(src(rel), sig)
        if b is None:
            ok = False
            out.append("FAIL completion function not found exactly once: %s" % sig)
            continue
        lines = [l for l in b.split("\n") if l.strip() not in ALLOWED]
        hits = sorted(set(t for t in FORBIDDEN for l in lines if t in l))
        if hits:
            ok = False
            out.append("FAIL live read at completion in %s: %s" % (sig.split("(")[0].split()[-1], ", ".join(hits)))
        else:
            out.append("ok   no live read in %s" % sig.split("(")[0].split()[-1])
    for rel, sig, needles, what in REQUIRED:
        b = body_of(src(rel), sig)
        missing = [x for x in needles if b is None or x not in b]
        if missing:
            ok = False
            out.append("FAIL %s (missing: %s)" % (what, "; ".join(missing)))
        else:
            out.append("ok   %s" % what)
    for rel, sig, needles, what in ABSENT:
        b = body_of(src(rel), sig)
        present = [x for x in needles if b is None or x in b]
        if present:
            ok = False
            out.append("FAIL %s (found: %s)" % (what, "; ".join(present)))
        else:
            out.append("ok   %s" % what)
    allowed_seen = sum(1 for rel, sig in COMPLETION for a in ALLOWED
                       if body_of(src(rel), sig) and a in body_of(src(rel), sig))
    out.append("note allowed completion-time live handle(s) seen: %d (the m26 mask-record handle; computes nothing for "
               "this frame's label)" % allowed_seen)
    return ok, out


MUTANTS = {
    "r4_live_annotation_inputs": (CAP, "AnomalyLabel::ProjectSnapshotFireBox(Snap, i, Min, Max)",
                                  "AnomalyViewport::ProjectActorBoundsToScreenRect(Snap.View, Snap.Fires[i].TargetActor.Get(), Min, Max)"),
    "r4_live_record_box": (LW, "AnomalyLabel::ProjectFrozenFireBox(Geometry, View, Min, Max)",
                           "AnomalyViewport::ProjectActorBoundsToScreenRect(View, F.TargetActor.Get(), Min, Max)"),
    "r4_live_coverage": (CAP, "AnomalyLabel::ProjectFrozenFireBox(\n\t\t\t\t(FireGeometry && FireGeometry->IsValidIndex(i)) ? (*FireGeometry)[i] : GUnsampled, View, Min, Max);",
                         "AnomalyViewport::ProjectActorBoundsToScreenRect(View, F.TargetActor.Get(), Min, Max);"),
    "r4_live_anchor": (CAP, "\t\t\t\tEv->NodePath = Anchor->NodePath;\n",
                       "\t\t\t\tEv->NodePath = F.TargetActor.Get() ? F.TargetActor.Get()->GetPathName() : Anchor->NodePath;\n"),
    "r4_sample_not_frozen": (CAP, "StepHideTransitions(*Snap);\n\tFreezeSampleGeometry(*Snap);", "StepHideTransitions(*Snap);"),
    "r4_sync_not_flagged": (CAP, "\tSyncFrame.bCaptureUnpaired = true;\n", "\n"),
    "r4_sync_not_marked": (CAP, "\tAnomalyLabel::MarkCaptureUnpaired(SyncFrame);\n", "\n"),
    "r4_record_labels_unpaired": (LW, "FireIndex != INDEX_NONE && !bCaptureUnpaired", "FireIndex != INDEX_NONE"),
}


def selftest(root):
    lines = []
    good = True

    def reader(over=None):
        def read(rel):
            t = _norm(open(os.path.join(root, rel), "r", encoding="utf-8", errors="replace").read())
            if over and over[0] == rel:
                n = t.count(over[1])
                if n != 1:
                    raise SystemExit("mutant anchor found %d times: %r" % (n, over[1][:80]))
                t = t.replace(over[1], over[2])
            return t
        return read

    ok, _ = check(reader())
    lines.append("SELFTEST %-52s %s" % ("the real source passes", "ok" if ok else "*** WRONG ***"))
    good = good and ok
    for name, (rel, a, b) in sorted(MUTANTS.items()):
        ok_m, out = check(reader((rel, a, b)))
        fired = [l for l in out if l.startswith("FAIL")]
        res = "FAILS-AS-INTENDED" if not ok_m else "PASSES-WRONGLY"
        lines.append("SELFTEST mutant %-45s %s (%s)" % (name, res, fired[0][5:90] if fired else "-"))
        good = good and not ok_m
    blind = check(lambda rel: "")[0]
    lines.append("SELFTEST %-52s %s" % ("an unreadable tree fails (no blind pass)", "ok" if not blind else "*** WRONG ***"))
    good = good and not blind
    for l in lines:
        print(l)
    print("R4 PAIRING GATE SELFTEST %d check(s): %s" % (len(lines), "OK" if good else "FAILED"))
    return 0 if good else 1


def main(argv=None):
    ap = argparse.ArgumentParser(description="090-10 R4 gate: label geometry is read at the sample, never at completion.")
    ap.add_argument("--root", default=os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args(argv)
    if a.selftest:
        return selftest(a.root)
    ok, out = check(lambda rel: open(os.path.join(a.root, rel), "r", encoding="utf-8", errors="replace").read())
    for l in out:
        print(l)
    print("R4 PAIRING GATE: %s" % ("PASS" if ok else "FAIL"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
