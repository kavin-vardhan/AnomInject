import argparse
import re
import sys
from pathlib import Path

PATHS = {
    "tree": "Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp",
    "vpcpp": "Source/AnomalyInjector/Private/AnomalyViewport.cpp",
    "vph": "Source/AnomalyInjector/Public/AnomalyViewport.h",
    "state": "Source/AnomalyInjector/Private/Anomalies/TexCorruptState.cpp",
    "core": "Source/AnomalyInjector/Private/Anomalies/TexCorruptCore.h",
    "stats": "Source/AnomalyInjector/Public/AnomalyTexCorrupt.h",
}

PAIRS = {"(": ")", "[": "]", "{": "}"}

CENSUS_SIG = r"\bvoid\s+RunOfficeCensus\s*\("
SCOPE_RX = re.compile(r"\bFReadOnlyEnumerationScope\s+\w+\s*;")
SCOPE_LINE_RX = r"[ \t]*(?:AnomalyViewport::)?FReadOnlyEnumerationScope\s+\w+\s*;[ \t]*\n"
ENUM_RX = [re.compile(p) for p in (
    r"\bTActorIterator\s*<",
    r"\bIsRenderableComponent\w*\s*\(",
    r"\bGetVisibleRenderableActor\w*\s*\(",
    r"\bGetCensusPrefilterActors\s*\(",
    r"\bEvaluateTree\s*\(",
    r"\bFind\w*Matching\w*\s*[<(]",
    r"\bResolveLodComponents\s*\(",
    r"\bGetActiveViewInfo\s*\(",
    r"\bFilter\w*Visible\w*\s*[<(]",
    r"\bClassifyRenderableVisibleLive\s*\(",
)]

NO_WORLD = "IAI.TexCorrupt.Census: no world; nothing was counted."
END_PREFIX = "IAI-TEXCORRUPT-CENSUS v1 end"
CENSUS_FORMATS = {
    NO_WORLD: 0,
    "IAI-TEXCORRUPT-CENSUS v1 scope=%s candidates=%d cap_bytes=%d uv_modes=%s normal_modes=%s": 5,
    "IAI-TEXCORRUPT-CENSUS v1 id=%s eligible=%d refused=%d reasons=%s": 4,
    "IAI-TEXCORRUPT-CENSUS v1 end stats_unchanged=%d": 1,
}
PRINTF_FORMATS = {"%s%s:%d"}


def norm(s):
    return re.sub(r"\s+", "", s)


ARG_ALLOW = {norm(a) for a in (
    'bAll ? TEXT("all") : TEXT("view")',
    "Names.Num()",
    "GetMaxRtBytes()",
    "*DescribeModeSet(EFamily::UV, GetEnabledModeMask(EFamily::UV))",
    "*DescribeModeSet(EFamily::Normal, GetEnabledModeMask(EFamily::Normal))",
    'f == 0 ? TEXT("uv_corruption") : TEXT("normal_corruption")',
    "Counts[f].Eligible",
    "Counts[f].Refused",
    'Reasons.IsEmpty() ? TEXT("-") : *Reasons',
    "bStatsUnchanged ? 1 : 0",
)}
PRINTF_ARG_ALLOW = {norm(a) for a in (
    'Reasons.IsEmpty() ? TEXT("") : TEXT(",")',
    "*Key",
    "Counts[f].Reasons[Key]",
)}
NAME_TOKENS = ("GetName", "GetPathName", "GetFullName", "GetNameSafe", "Names[", "*Name", "Name)", "Name,", "Actor",
               "Prim", "Comp", "Weak", "Path", "Label", "TargetQuery", ".Sub", "Slots", "Bindings", "ToString", "World",
               "Texture", "Asset")
OTHER_LOG_RX = re.compile(r"\b(?:UE_CLOG|UE_LOGFMT|GLog|FMsg|GEngine)\b|\bLogf\s*\(|\bprintf\s*\(|AddOnScreenDebugMessage")

MUTATOR_RX = [re.compile(p) for p in (
    r"\bCountTreeDispositions\s*\(",
    r"\bFStatsAccess\s*::\s*Mutable\b",
    r"\bResetTargetExclusionStats\s*\(",
    r"\bResetRunStats\s*\(",
    r"\bCountReason\s*\(",
    r"\bTakeAttemptOrdinal\s*\(",
    r"\bTripwireCounter\s*\(",
    r"\bLevers\s*\(\s*\)",
    r"\bLedger\s*\(\s*\)\s*\.\s*(?:Reserve|ForceReserve|Unreserve|ReleaseToPending|Tick|NotePeak)\s*\(",
    r"\b(?:Reserve|ForceReserve|Unreserve|ReleaseToPending|ReleaseCreated|NoteCreated|Close)\s*\(\s*Ledger\s*\(",
    r"\bSet(?:AnomalyEnabled|AllAnomaliesEnabled|AutoPoolSelection|Seed|Running|Enabled|PollRadius|MinScreenCoveragePct|"
    r"OverlaysSuppressed|ViewportScoping\w*|ExcludedTargets\w*|AllowTranslucentOnlyTargets\w*|IntervalRange|HoldRange|"
    r"MaxConcurrent|Persist)\s*\(",
    r"\b(?:TryFireOnce|TryFireSpecific|ApplyAnomaly|RevertAnomaly|RevertAll\w*|BeginWarmDraw|EndWarmDraw|"
    r"RestoreBenchAssetSlotMid)\s*\(",
    r"\bG(?:Stats|Ledger|Ordinals|Levers|Tripwire)\b",
)]

GUARD_RETURN_RX = re.compile(
    r"if\s*\(\s*(?:AnomalyViewport\s*::\s*)?IsReadOnlyEnumeration\s*\(\s*\)\s*\)\s*\{\s*return\s+true\s*;\s*\}")
GUARD_BLOCK_RX = re.compile(r"if\s*\(\s*!\s*(?:AnomalyViewport\s*::\s*)?IsReadOnlyEnumeration\s*\(\s*\)\s*\)\s*\{")
SIDE_CHANNELS = (
    ("MatchesExcludedTargetPattern", r"\bbool\s+MatchesExcludedTargetPattern\s*\(",
     r"\bExcludedActorsSeen\s*\(\s*\)\s*\.\s*Add\s*\(", "EXCLUDED-TARGET", r"\bif\s*\(\s*Field\s*\)"),
    ("RefusedAsTranslucentOnly", r"\bbool\s+RefusedAsTranslucentOnly\s*\(",
     r"\bTranslucentOnlyActorsSeen\s*\(\s*\)\s*\.\s*Add\s*\(", "EXCLUDED-TRANSLUCENT",
     r"\bIsActorRenderableTranslucentOnly\s*\("),
)
READ_ONLY_ENTRIES = (
    ("IsRenderableComponentReadOnly", r"\bIsRenderableComponent\s*\("),
    ("GetVisibleRenderableActorsReadOnly", r"\bGetVisibleRenderableActors\s*\("),
)

DIGEST_SIG = r"\buint32\s+RunStatsDigest\s*\("
DIGEST_HELPER_SIGS = (r"\bvoid\s+RunStatsDigestFold\s*\(", r"\bvoid\s+RunStatsDigestFoldMap\s*\(")
PURITY_RX = [re.compile(p) for p in (
    r"\bFStatsAccess\s*::\s*Mutable\b",
    r"\bG(?:Stats|Ledger|Ordinals|Levers)\b(?:\s*\.\s*\w+|\s*\[[^\]]*\])*\s*(?:=(?!=)|\+=|-=|\*=|\+\+|--)",
    r"(?:\+\+|--)\s*G(?:Stats|Ledger|Ordinals|Levers)\b",
    r"\bGLedger\s*\.\s*(?:Tick|Reserve|ForceReserve|Unreserve|ReleaseToPending|NotePeak)\s*\(",
    r"\bGOrdinals\s*\.\s*(?:Take|Reset)\s*\(",
    r"\bGTripwire\s*\.\s*(?:Increment|Decrement|Add|Subtract|Set|Reset)\s*\(",
    r"\b(?:TakeAttemptOrdinal|ResetRunStats|EchoRunStartKnobs|CountReason|Ledger|Levers|TripwireCounter)\s*\(",
)]
DIGEST_EXTRA = (r"\bGTripwire\s*\.\s*GetValue\s*\(", r"\bGLedger\s*\.\s*Live\b", r"\bGLedger\s*\.\s*Peak\b",
                r"\bGLedger\s*\.\s*Buckets\b", r"\bGLedger\s*\.\s*Due\b", r"\bGLedger\s*\.\s*Bytes\b",
                r"\bGOrdinals\s*\.\s*Next\b")


def skip_literal(text, i):
    q = text[i]
    j = i + 1
    n = len(text)
    while j < n:
        c = text[j]
        if c == "\\":
            j += 2
            continue
        if c == q or c == "\n":
            return j + 1
        j += 1
    return n


def skip_trivia(text, i):
    if text.startswith("//", i):
        j = text.find("\n", i)
        return len(text) if j < 0 else j
    if text.startswith("/*", i):
        j = text.find("*/", i + 2)
        return len(text) if j < 0 else j + 2
    if text[i] in "\"'":
        return skip_literal(text, i)
    return i


def match_close(text, open_idx):
    opener = text[open_idx]
    closer = PAIRS[opener]
    depth = 0
    i = open_idx
    n = len(text)
    while i < n:
        j = skip_trivia(text, i)
        if j != i:
            i = j
            continue
        c = text[i]
        if c == opener:
            depth += 1
        elif c == closer:
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return -1


def brace_depth(text, pos):
    depth = 0
    i = 0
    while i < pos:
        j = skip_trivia(text, i)
        if j != i:
            i = j
            continue
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
        i += 1
    return depth


def definitions(text, sig):
    out = []
    for m in re.finditer(sig, text):
        p = m.end() - 1
        if text[p] != "(":
            continue
        close = match_close(text, p)
        if close < 0:
            continue
        k = close + 1
        tail = re.match(r"\s*(?:const\s*)?(?:override\s*)?(?:noexcept\s*)?", text[k:k + 64])
        k += tail.end()
        if k < len(text) and text[k] == "{":
            end = match_close(text, k)
            if end > 0:
                out.append((k + 1, end))
    return out


def find_calls(text, rx):
    out = []
    for m in rx.finditer(text):
        p = m.end() - 1
        close = match_close(text, p)
        if close < 0:
            continue
        out.append((m.start(), text[p + 1:close]))
    return out


def split_args(s):
    args = []
    depth = 0
    start = 0
    i = 0
    while i < len(s):
        j = skip_trivia(s, i)
        if j != i:
            i = j
            continue
        c = s[i]
        if c in "([{":
            depth += 1
        elif c in ")]}":
            depth -= 1
        elif c == "," and depth == 0:
            args.append(s[start:i].strip())
            start = i + 1
        i += 1
    tail = s[start:].strip()
    if tail or args:
        args.append(tail)
    return args


def format_of(expr):
    if not re.fullmatch(r'(?:\s*TEXT\s*\(\s*"(?:[^"\\]|\\.)*"\s*\)\s*)+', expr):
        return None
    return "".join(re.findall(r'TEXT\s*\(\s*"((?:[^"\\]|\\.)*)"\s*\)', expr))


def spec_count(fmt):
    return sum(1 for m in re.finditer(r"%(?:%|[-+ #0]*\d*(?:\.\d+)?(?:ll|l|h|z)?[sdiufxXcpeEgG])", fmt)
               if m.group(0) != "%%")


def struct_fields(text, name):
    m = re.search(r"\bstruct\s+" + name + r"\s*\{", text)
    if not m:
        return None
    close = match_close(text, m.end() - 1)
    body = text[m.end():close]
    return re.findall(r"^\s*[\w:<>, ]+?\s+(\w+)\s*(?:=[^;]*)?;", body, re.M)


def check_census(tree, failures):
    defs = definitions(tree, CENSUS_SIG)
    if len(defs) != 1:
        failures.append(f"(a) RunOfficeCensus definitions: {len(defs)} (expected exactly 1)")
        return
    s, e = defs[0]
    body = tree[s:e]

    enums = sorted((m.start(), m.group(0)) for rx in ENUM_RX for m in rx.finditer(body))
    scopes = [m.start() for m in SCOPE_RX.finditer(body) if brace_depth(body, m.start()) == 0]
    if not enums:
        failures.append("(a) RunOfficeCensus: no enumeration call found; there is nothing for the scope to guard")
    if not scopes:
        failures.append("(a) RunOfficeCensus: no FReadOnlyEnumerationScope is held at the top level of the body")
    elif enums and scopes[0] > enums[0][0]:
        failures.append(f"(a) RunOfficeCensus: the read-only scope opens after the first enumeration call '{enums[0][1]}'")

    parsed = []
    for pos, argtext in find_calls(body, re.compile(r"\bUE_LOG\s*\(")):
        args = split_args(argtext)
        fmt = format_of(args[2]) if len(args) >= 3 else None
        if fmt is None:
            failures.append(f"(b) RunOfficeCensus: a UE_LOG whose format is not a literal: {norm(argtext)[:80]}")
            continue
        parsed.append((pos, fmt, args[3:]))
    seen = {}
    for pos, fmt, fargs in parsed:
        if fmt not in CENSUS_FORMATS:
            failures.append(f"(b) RunOfficeCensus: a UE_LOG that is not a census line or the no-world warning: '{fmt}'")
        else:
            seen[fmt] = seen.get(fmt, 0) + 1
            if spec_count(fmt) != len(fargs) or len(fargs) != CENSUS_FORMATS[fmt]:
                failures.append(f"(b) RunOfficeCensus: '{fmt}' has {len(fargs)} argument(s) for {spec_count(fmt)} specifier(s)")
        for a in fargs:
            if norm(a) not in ARG_ALLOW:
                named = [t for t in NAME_TOKENS if t in a]
                tag = f" (name-bearing: {', '.join(named)})" if named else ""
                failures.append(f"(b) RunOfficeCensus: a UE_LOG argument not on the count-only allowlist{tag}: {a}")
    for fmt in CENSUS_FORMATS:
        if fmt.startswith(END_PREFIX):
            continue
        if seen.get(fmt, 0) != 1:
            failures.append(f"(b) RunOfficeCensus: '{fmt}' appears {seen.get(fmt, 0)} time(s) (expected 1)")
    other = OTHER_LOG_RX.search(body)
    if other:
        failures.append(f"(b) RunOfficeCensus: a second logging route '{other.group(0)}'")
    for pos, argtext in find_calls(body, re.compile(r"\bFString\s*::\s*Printf\s*\(")):
        args = split_args(argtext)
        fmt = format_of(args[0]) if args else None
        if fmt not in PRINTF_FORMATS:
            failures.append(f"(b) RunOfficeCensus: an FString::Printf with an unlisted format: {fmt}")
            continue
        for a in args[1:]:
            if norm(a) not in PRINTF_ARG_ALLOW:
                failures.append(f"(b) RunOfficeCensus: an FString::Printf argument not on the allowlist: {a}")
    for pos, argtext in find_calls(body, re.compile(r"\.\s*FindOrAdd\s*\(")):
        if norm(argtext) != "Result.Reason":
            failures.append(f"(b) RunOfficeCensus: the reasons histogram is keyed by '{argtext}', not Result.Reason alone")

    for rx in MUTATOR_RX:
        m = rx.search(body)
        if m:
            failures.append(f"(c) RunOfficeCensus: calls a stats or pool mutator '{m.group(0)}'")
    for m in re.finditer(r"\bFTreeInputs\s+(\w+)\s*;", body):
        v = m.group(1)
        if not re.search(r"\b" + re.escape(v) + r"\s*\.\s*bCensus\s*=\s*true\s*;", body):
            failures.append(f"(c) RunOfficeCensus: FTreeInputs '{v}' is not put in census mode")

    ends = [(pos, fmt, fargs) for pos, fmt, fargs in parsed if fmt.startswith(END_PREFIX)]
    end_pos = None
    if len(ends) != 1:
        failures.append(f"(e) RunOfficeCensus: end lines: {len(ends)} (expected exactly 1)")
    else:
        end_pos, fmt, fargs = ends[0]
        if "stats_unchanged=%d" not in fmt:
            failures.append("(e) RunOfficeCensus: the end line does not carry stats_unchanged=")
        elif [norm(a) for a in fargs] != ["bStatsUnchanged?1:0"]:
            failures.append("(e) RunOfficeCensus: stats_unchanged= is not fed by bStatsUnchanged")
    lam = re.search(r"\bauto\s+TakeStatsSnapshot\s*=\s*\[[^\]]*\]\s*\(\s*\)\s*\{", body)
    if not lam:
        failures.append("(e) RunOfficeCensus: no TakeStatsSnapshot lambda")
    else:
        lclose = match_close(body, lam.end() - 1)
        lbody = body[lam.end():lclose]
        for need in ("GetTargetExclusionCount", "GetTranslucentOnlyExclusionCount", "RunStatsDigest"):
            if not re.search(r"\b" + need + r"\s*\(\s*\)", lbody):
                failures.append(f"(e) RunOfficeCensus: the snapshot does not read {need}()")
        calls = [m.start() for m in re.finditer(r"\bTakeStatsSnapshot\s*\(\s*\)", body)]
        evals = [m.start() for m in re.finditer(r"\bEvaluateTree\s*\(", body)]
        if len(calls) < 2:
            failures.append(f"(e) RunOfficeCensus: TakeStatsSnapshot() is called {len(calls)} time(s) (expected before and after)")
        else:
            if enums and calls[0] > enums[0][0]:
                failures.append("(e) RunOfficeCensus: the before-snapshot is taken after enumeration began")
            if evals and calls[-1] < evals[-1]:
                failures.append("(e) RunOfficeCensus: the after-snapshot is taken before the last EvaluateTree")
            if end_pos is not None and calls[-1] > end_pos:
                failures.append("(e) RunOfficeCensus: the after-snapshot is taken after the end line")
    fields = struct_fields(body, "FStatsSnapshot")
    cmp_m = re.search(r"\bbool\s+bStatsUnchanged\s*=([^;]*);", body)
    if not fields or not cmp_m:
        failures.append("(e) RunOfficeCensus: no FStatsSnapshot struct or no bStatsUnchanged comparison")
    else:
        expr = norm(cmp_m.group(1))
        for f in fields:
            if f"StatsBefore.{f}==StatsAfter.{f}" not in expr:
                failures.append(f"(e) RunOfficeCensus: snapshot field {f} is not compared before/after")


def check_viewport(vp, vph, failures):
    for name, sig, add_rx, log_tag, anchor_rx in SIDE_CHANNELS:
        defs = definitions(vp, sig)
        if len(defs) != 1:
            failures.append(f"(d) {name}: definitions: {len(defs)} (expected exactly 1)")
            continue
        s, e = defs[0]
        body = vp[s:e]
        adds = [m.start() for m in re.finditer(add_rx, body)]
        logs = [m.start() for m in re.finditer(re.escape(log_tag), body)]
        total_adds = len(re.findall(add_rx, vp))
        total_logs = vp.count(log_tag)
        if len(adds) != 1 or total_adds != 1:
            failures.append(f"(d) {name}: set Add sites {len(adds)} in the function, {total_adds} in the file (expected 1 and 1)")
        if len(logs) != 1 or total_logs != 1:
            failures.append(f"(d) {name}: {log_tag} logs {len(logs)} in the function, {total_logs} in the file (expected 1 and 1)")
        if not adds or not logs:
            continue
        guards = [m.start() for m in GUARD_RETURN_RX.finditer(body)]
        dominating = [g for g in guards if g < adds[0] and g < logs[0] and brace_depth(body, g) == brace_depth(body, adds[0])]
        anchor = re.search(anchor_rx, body)
        if not guards:
            failures.append(f"(d) {name}: no read-only guard before the set Add and the {log_tag} log")
        elif not dominating:
            failures.append(f"(d) {name}: the read-only guard does not sit in the recording block ahead of the Add and the log")
        elif anchor is None or dominating[-1] < anchor.start():
            failures.append(f"(d) {name}: the read-only guard precedes the eligibility test, so read-only mode changes the answer")
    for m in re.finditer(r"\b(ExcludedActorsSeen|TranslucentOnlyActorsSeen)\s*\(\s*\)\s*\.\s*(\w+)", vp):
        if m.group(2) not in ("Add", "Reset", "Num", "Contains"):
            failures.append(f"(d) {m.group(1)}().{m.group(2)} is a writer outside the guarded recording block")
    reset_defs = definitions(vp, r"\bvoid\s+ResetTargetExclusionStats\s*\(")
    reset_body = vp[reset_defs[0][0]:reset_defs[0][1]] if len(reset_defs) == 1 else ""
    if len(re.findall(r"Seen\s*\(\s*\)\s*\.\s*Reset\s*\(", vp)) != len(re.findall(r"Seen\s*\(\s*\)\s*\.\s*Reset\s*\(", reset_body)):
        failures.append("(d) an exclusion set is Reset outside ResetTargetExclusionStats")

    defs = definitions(vp, r"\bbool\s+GetActiveViewInfo\s*\(")
    if len(defs) != 1:
        failures.append(f"(d) GetActiveViewInfo: definitions: {len(defs)} (expected exactly 1)")
    else:
        body = vp[defs[0][0]:defs[0][1]]
        blocks = [(m.end() - 1, match_close(body, m.end() - 1)) for m in GUARD_BLOCK_RX.finditer(body)]
        for pos, argtext in find_calls(body, re.compile(r"\bUE_LOG\s*\(")):
            if re.search(r"\bGet(?:Name|NameSafe|PathName|FullName)\s*\(", argtext):
                if not any(o < pos < c for o, c in blocks):
                    failures.append("(d) GetActiveViewInfo: a name-bearing warning is not inside if (!IsReadOnlyEnumeration())")

    if not re.search(r"\bstruct\s+ANOMALYINJECTOR_API\s+FReadOnlyEnumerationScope\b", vph):
        failures.append("(d) AnomalyViewport.h: no exported FReadOnlyEnumerationScope")
    if not re.search(r"\bANOMALYINJECTOR_API\s+bool\s+IsReadOnlyEnumeration\s*\(\s*\)\s*;", vph):
        failures.append("(d) AnomalyViewport.h: no exported IsReadOnlyEnumeration()")
    if not re.search(r"\bthread_local\s+int32\s+GReadOnlyEnumerationDepth\s*=\s*0\s*;", vp):
        failures.append("(d) the depth counter is not a zero-initialised thread_local int32")
    for label, sig, want in (
        ("constructor", r"\bFReadOnlyEnumerationScope\s*::\s*FReadOnlyEnumerationScope\s*\(", "++GReadOnlyEnumerationDepth;"),
        ("destructor", r"\bFReadOnlyEnumerationScope\s*::\s*~\s*FReadOnlyEnumerationScope\s*\(", "--GReadOnlyEnumerationDepth;"),
        ("IsReadOnlyEnumeration", r"\bbool\s+IsReadOnlyEnumeration\s*\(", "returnGReadOnlyEnumerationDepth>0;"),
    ):
        d = definitions(vp, sig)
        if len(d) != 1 or norm(vp[d[0][0]:d[0][1]]) != norm(want):
            failures.append(f"(d) the scope {label} is not exactly '{want}'")
    for fn, call in READ_ONLY_ENTRIES:
        d = definitions(vp, r"\b" + fn + r"\s*\(")
        if len(d) != 1:
            failures.append(f"(d) {fn}: definitions: {len(d)} (expected exactly 1)")
            continue
        body = vp[d[0][0]:d[0][1]]
        scope = [m.start() for m in SCOPE_RX.finditer(body) if brace_depth(body, m.start()) == 0]
        c = re.search(call, body)
        if not scope or not c or scope[0] > c.start():
            failures.append(f"(d) {fn}: does not open the read-only scope before delegating")


def check_digest(state, core, stats_h, failures):
    if not re.search(r"\buint32\s+RunStatsDigest\s*\(\s*\)\s*;", core):
        failures.append("(f) TexCorruptCore.h: no RunStatsDigest() declaration")
    defs = definitions(state, DIGEST_SIG)
    if len(defs) != 1:
        failures.append(f"(f) RunStatsDigest definitions: {len(defs)} (expected exactly 1)")
        return
    body = state[defs[0][0]:defs[0][1]]
    pure_text = body
    for sig in DIGEST_HELPER_SIGS:
        for s, e in definitions(state, sig):
            pure_text += "\n" + state[s:e]
    for rx in PURITY_RX:
        m = rx.search(pure_text)
        if m:
            failures.append(f"(f) RunStatsDigest is not a pure read: '{m.group(0)}'")
    fields = struct_fields(stats_h, "FRunStats")
    if not fields:
        failures.append("(f) AnomalyTexCorrupt.h: FRunStats not found")
    else:
        for f in fields:
            if not re.search(r"\bGStats\s*\.\s*" + f + r"\b", body):
                failures.append(f"(f) run-stats field {f} is not folded into RunStatsDigest")
    for need in DIGEST_EXTRA:
        if not re.search(need, body):
            failures.append(f"(f) RunStatsDigest does not fold {need}")


def check(files):
    failures = []
    check_census(files["tree"], failures)
    check_viewport(files["vpcpp"], files["vph"], failures)
    check_digest(files["state"], files["core"], files["stats"], failures)
    return failures


def mutate(files, key, sig, fn):
    text = files[key]
    if sig is None:
        new = fn(text)
    else:
        defs = definitions(text, sig)
        if len(defs) != 1:
            return None
        s, e = defs[0]
        seg = fn(text[s:e])
        new = None if seg is None else text[:s] + seg + text[e:]
    if new is None or new == text:
        return None
    out = dict(files)
    out[key] = new
    return out


def rep(old, new):
    return lambda t: t.replace(old, new, 1) if old in t else None


def rx_sub(pattern, new):
    rx = re.compile(pattern)
    return lambda t: rx.sub(lambda m: new, t, count=1) if rx.search(t) else None


def prepend(s):
    return lambda t: s + t


def chain(*fns):
    def run(t):
        for fn in fns:
            if t is None:
                return None
            t = fn(t)
        return t
    return run


def build_mutants(files):
    scope_stmt = "\t\tAnomalyViewport::FReadOnlyEnumerationScope ReadOnly;\n"
    guard = "\n\t\tif (AnomalyViewport::IsReadOnlyEnumeration())\n\t\t{\n\t\t\treturn true;\n\t\t}"
    name_log = "\t\t\tUE_LOG(LogAnomaly, Display, TEXT(\"IAI-TEXCORRUPT-CENSUS v1 candidate=%s\"), *Name);\n"
    return [
        ("scope removed from the census", "(a)",
         mutate(files, "tree", CENSUS_SIG, rx_sub(SCOPE_LINE_RX, ""))),
        ("scope moved after the enumeration", "(a)",
         mutate(files, "tree", CENSUS_SIG, chain(rx_sub(SCOPE_LINE_RX, ""), rep("\t\tNames.Sort();", scope_stmt + "\t\tNames.Sort();")))),
        ("scope confined to the 'all' block", "(a)",
         mutate(files, "tree", CENSUS_SIG, chain(rx_sub(SCOPE_LINE_RX, ""), rx_sub(r"if \(bAll\)\n\t\t\{\n", "if (bAll)\n\t\t{\n\t" + scope_stmt)))),
        ("name-bearing UE_LOG added to the census", "(b)",
         mutate(files, "tree", CENSUS_SIG, rx_sub(r"for \(const FString& Name : Names\)\n\t\t\{\n",
                                                   "for (const FString& Name : Names)\n\t\t{\n" + name_log))),
        ("reasons histogram keyed by the target", "(b)",
         mutate(files, "tree", CENSUS_SIG, rep(".FindOrAdd(Result.Reason)", ".FindOrAdd(Result.Reason + In.TargetQuery)"))),
        ("CountTreeDispositions call added to the census", "(c)",
         mutate(files, "tree", CENSUS_SIG, rep("EvaluateTree(World, In, Result);", "EvaluateTree(World, In, Result);\n\t\t\t\tCountTreeDispositions(Result);"))),
        ("guard removed from MatchesExcludedTargetPattern", "(d)",
         mutate(files, "vpcpp", SIDE_CHANNELS[0][1], rx_sub(GUARD_RETURN_RX.pattern, ""))),
        ("guard hoisted above the pattern match", "(d)",
         mutate(files, "vpcpp", SIDE_CHANNELS[0][1], chain(rx_sub(GUARD_RETURN_RX.pattern, ""), prepend(guard)))),
        ("guard removed from RefusedAsTranslucentOnly", "(d)",
         mutate(files, "vpcpp", SIDE_CHANNELS[1][1], rx_sub(GUARD_RETURN_RX.pattern, ""))),
        ("second, unguarded translucent writer", "(d)",
         mutate(files, "vpcpp", r"\bint32\s+GetTranslucentOnlyExclusionCount\s*\(",
                rep("return TranslucentOnlyActorsSeen().Num();", "TranslucentOnlyActorsSeen().Add(FString());\n\t\treturn TranslucentOnlyActorsSeen().Num();"))),
        ("view warning left unguarded", "(d)",
         mutate(files, "vpcpp", r"\bbool\s+GetActiveViewInfo\s*\(", rx_sub(r"if\s*\(\s*!\s*IsReadOnlyEnumeration\s*\(\s*\)\s*\)", "if (true)"))),
        ("scope constructor does not count", "(d)",
         mutate(files, "vpcpp", r"\bFReadOnlyEnumerationScope\s*::\s*FReadOnlyEnumerationScope\s*\(", rep("++GReadOnlyEnumerationDepth;", ""))),
        ("stats_unchanged removed from the end line", "(e)",
         mutate(files, "tree", CENSUS_SIG, rx_sub(r'UE_LOG\(LogAnomaly, Display, TEXT\("IAI-TEXCORRUPT-CENSUS v1 end[^"]*"\)[^;]*;',
                                                   'UE_LOG(LogAnomaly, Display, TEXT("IAI-TEXCORRUPT-CENSUS v1 end"));'))),
        ("a snapshot field left uncompared", "(e)",
         mutate(files, "tree", CENSUS_SIG, rx_sub(r"\s*&&\s*StatsBefore\.PoolIds\s*==\s*StatsAfter\.PoolIds", ""))),
        ("a run-stats field dropped from the digest", "(f)",
         mutate(files, "state", DIGEST_SIG, rx_sub(r"[^\n]*GStats\.Swept[^\n]*\n", ""))),
        ("digest takes an attempt ordinal", "(f)",
         mutate(files, "state", DIGEST_SIG, prepend("\n\t\tTakeAttemptOrdinal(EFamily::UV);"))),
    ]


def report(label, failures):
    if failures:
        for f in failures:
            print(f"CENSUS-READONLY {label} FAIL {f}")
        print(f"CENSUS-READONLY {label} VERDICT FAIL ({len(failures)} failure(s))")
    else:
        print(f"CENSUS-READONLY {label} VERDICT PASS (the office census holds the read-only scope over its whole body, "
              f"prints counts only, calls no stats or pool mutator, both exclusion side channels and the named view warning "
              f"are guarded, and the end line carries stats_unchanged= from a before/after snapshot of a pure digest)")


def selftest(files):
    ok = True
    clean = check(files)
    print(f"CENSUS-READONLY SELFTEST clean copy -> {'PASS' if not clean else 'FAIL'}")
    for f in clean:
        print(f"CENSUS-READONLY SELFTEST   clean failure: {f}")
    ok &= not clean
    mutants = build_mutants(files)
    for name, clause, mutant in mutants:
        if mutant is None:
            print(f"CENSUS-READONLY SELFTEST NOT-BUILT {name} (the planted edit did not apply to this tree)")
            ok = False
            continue
        f = check(mutant)
        hit = [x for x in f if x.startswith(clause)]
        if hit:
            print(f"CENSUS-READONLY SELFTEST OK {name} -> FAILED as expected {clause} [{hit[0]}]")
        elif f:
            print(f"CENSUS-READONLY SELFTEST WRONG-CLAUSE {name} -> failed, but not on {clause} [{f[0]}]")
            ok = False
        else:
            print(f"CENSUS-READONLY SELFTEST BLIND {name} -> PASSED (the check is blind)")
            ok = False
    print(f"CENSUS-READONLY SELFTEST {1 + len(mutants)} case(s): {'OK' if ok else 'FAILED'}")
    return ok


def main():
    ap = argparse.ArgumentParser(description="IAI.TexCorrupt.Census is read-only: counts only, no names, no stats or pool mutation.")
    ap.add_argument("--root", default=str(Path(__file__).resolve().parent.parent))
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    root = Path(a.root)
    files = {k: (root / p).read_text(encoding="utf-8", errors="surrogateescape").replace("\r\n", "\n")
             for k, p in PATHS.items()}
    if a.selftest:
        sys.exit(0 if selftest(files) else 1)
    failures = check(files)
    report(str(root), failures)
    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
