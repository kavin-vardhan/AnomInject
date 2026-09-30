#!/usr/bin/env python3
"""G-LEVER-AUDIT: every bench lever and every bench catalogue entry is behind the one bench gate.

Reads the plugin's Source tree and descriptor (never a guess about them) and asserts:

  R1  no "IAI.Bench." string is live in a Shipping compile of a module Shipping builds;
  R2  every IAI.Bench.* registration live in Development or Editor is either a file-scope static
      (masked by AnomalyBenchGate::SweepUngatedLevers when the gate is shut) or a runtime
      registration reached only through AnomalyBenchGate::IsEnabled();
  R3  the sweep runs at UAnomalyInjectorSubsystem::Initialize and at OnPostEngineInit, and the
      gate opens on -IAIBench and -IAIBenchFixture and is false in Shipping;
  R4  bench catalogue entries (anomaly classes that require -IAIBenchFixture, and the twin ids)
      register only inside the gate and never in Shipping, and no pool lists them;
  R5  levers are looked up by name only through AnomalyBenchGate::FindLeverCommand (or the
      read-only ChangeSettingSource report);
  R6  no second gate: no inline FParse::Param(..., TEXT("IAIBench")) outside the gate.

With --binary it also checks a built artifact against the source: every lever name the binary
carries is one the source compiles in that configuration, every registered lever name is in the
binary (a stale build fails), and the gate's echo strings are present.

--selftest copies the tree, plants one violation per rule, and requires the clean copy to PASS
and every planted copy to FAIL on its rule. Since 090-07 every plugin module is denied to Shipping in
the descriptor, so the R1 plants re-admit their module to Shipping in the copy (R1 is about a module
Shipping builds), and one more case requires the same R1 plant in a module the descriptor keeps out of
Shipping to PASS.

Exit 0 PASS, 1 FAIL, 2 cannot read the tree.
"""
import argparse
import bisect
import json
import os
import re
import shutil
import sys
import tempfile

PREFIX = "IAI.Bench."
GATE_CALL = "AnomalyBenchGate::IsEnabled()"
SWEEP_CALL = "AnomalyBenchGate::SweepUngatedLevers("
GATE_IMPL = os.path.join("Source", "AnomalyInjector", "Private", "AnomalyBenchGate.cpp")
TWIN_IDS = ("null_effect", "solid_swap")
ALLOWED_LOOKUPS = ("FindLeverCommand", "ChangeSettingSource")
RAW_LOOKUPS = ("FindConsoleObject", "FindConsoleVariable", "FindConsoleObjectUnfiltered",
               "FindTConsoleVariableDataInt", "FindTConsoleVariableDataFloat", "UnregisterConsoleObject")
REG_RX = re.compile(r"(?:FAutoConsole\w*|TAutoConsoleVariable\s*<[^<>;]*>|RegisterConsole\w*)\s*>?\s*(?:\w+\s*)?\(\s*(?:TEXT\s*\(\s*)?$")
CALL_RX = re.compile(r"(\w+)\s*\(\s*(?:TEXT\s*\(\s*)?$")
NAME_RX = re.compile(r"IAI\.Bench\.[A-Za-z0-9_]+")
ECHO_STRINGS = ("IAI bench levers: ENABLED", "IAI bench levers: DISABLED")

BASE = {"UE_BUILD_DEBUG": 0, "UE_BUILD_TEST": 0, "WITH_DEV_AUTOMATION_TESTS": 0}
CONFIGS = {
    "Shipping": dict(BASE, UE_BUILD_SHIPPING=1, UE_BUILD_DEVELOPMENT=0, ANOMALY_CAPTURE=0,
                     ANOMALY_CONTROL_SERVER=0, WITH_EDITOR=0, WITH_EDITORONLY_DATA=0),
    "Development": dict(BASE, UE_BUILD_SHIPPING=0, UE_BUILD_DEVELOPMENT=1, ANOMALY_CAPTURE=1,
                        ANOMALY_CONTROL_SERVER=1, WITH_EDITOR=0, WITH_EDITORONLY_DATA=0),
    "Editor": dict(BASE, UE_BUILD_SHIPPING=0, UE_BUILD_DEVELOPMENT=1, ANOMALY_CAPTURE=1,
                   ANOMALY_CONTROL_SERVER=1, WITH_EDITOR=1, WITH_EDITORONLY_DATA=1),
}


def and3(a, b):
    if a is False or b is False:
        return False
    if a is None or b is None:
        return None
    return True


def or3(a, b):
    if a is True or b is True:
        return True
    if a is None or b is None:
        return None
    return False


def not3(a):
    return None if a is None else (not a)


def eval_pp(expr, macros):
    e = re.sub(r"/\*.*?\*/", " ", expr)
    e = re.sub(r"//.*", " ", e)
    unknown = []

    def rep_defined(m):
        name = m.group(1) or m.group(2)
        if name in macros:
            return "1"
        unknown.append(name)
        return "0"

    e = re.sub(r"\bdefined\s*\(\s*(\w+)\s*\)|\bdefined\s+(\w+)", rep_defined, e)

    def rep_ident(m):
        name = m.group(0)
        if name in macros:
            return str(int(macros[name]))
        unknown.append(name)
        return "0"

    e = re.sub(r"\b[A-Za-z_]\w*\b", rep_ident, e)
    if unknown:
        return None
    e = e.replace("&&", " and ").replace("||", " or ")
    e = re.sub(r"!(?!=)", " not ", e)
    try:
        return bool(eval(e, {"__builtins__": {}}, {}))
    except Exception:
        return None


def line_activity(lines, macros):
    out = []
    stack = []
    i = 0
    while i < len(lines):
        raw = lines[i]
        stripped = raw.strip()
        j = i
        while stripped.startswith("#") and stripped.endswith("\\") and j + 1 < len(lines):
            j += 1
            stripped = stripped[:-1] + " " + lines[j].strip()
        cur = stack[-1]["active"] if stack else True
        m = re.match(r"#\s*(if|ifdef|ifndef|elif|else|endif)\b(.*)", stripped)
        if m:
            kind, rest = m.group(1), m.group(2).strip()
            if kind in ("if", "ifdef", "ifndef"):
                if kind == "if":
                    v = eval_pp(rest, macros)
                elif kind == "ifdef":
                    v = True if rest in macros else None
                else:
                    v = False if rest in macros else None
                stack.append({"parent": cur, "taken": v, "active": and3(cur, v)})
            elif kind == "elif" and stack:
                f = stack[-1]
                v = eval_pp(rest, macros)
                f["active"] = and3(f["parent"], and3(not3(f["taken"]), v))
                f["taken"] = or3(f["taken"], v)
            elif kind == "else" and stack:
                f = stack[-1]
                f["active"] = and3(f["parent"], not3(f["taken"]))
                f["taken"] = True
            elif kind == "endif" and stack:
                stack.pop()
            for _ in range(i, j + 1):
                out.append(False)
        else:
            for _ in range(i, j + 1):
                out.append(cur)
        i = j + 1
    return out


def scan(text):
    lits = []
    braces = []
    bounds = []
    i = 0
    n = len(text)
    while i < n:
        c = text[i]
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            k = text.find("\n", i)
            i = n if k < 0 else k
            continue
        if c == "/" and i + 1 < n and text[i + 1] == "*":
            k = text.find("*/", i + 2)
            i = n if k < 0 else k + 2
            continue
        if c == "R" and i + 1 < n and text[i + 1] == '"' and (i == 0 or not (text[i - 1].isalnum() or text[i - 1] == "_")):
            p = text.find("(", i + 2)
            delim = text[i + 2:p]
            end = text.find(")" + delim + '"', p)
            end = n if end < 0 else end
            lits.append((i, end + len(delim) + 2, text[p + 1:end]))
            i = end + len(delim) + 2
            continue
        if c == '"':
            j = i + 1
            while j < n and text[j] != '"':
                if text[j] == "\\":
                    j += 1
                if j < n and text[j] == "\n":
                    break
                j += 1
            lits.append((i, j + 1, text[i + 1:j]))
            i = j + 1
            continue
        if c == "'":
            j = i + 1
            while j < n and text[j] != "'":
                if text[j] == "\\":
                    j += 1
                if j < n and text[j] == "\n":
                    break
                j += 1
            i = j + 1
            continue
        if c in "{}":
            braces.append((i, c))
            bounds.append(i)
        elif c == ";":
            bounds.append(i)
        i += 1
    return lits, braces, bounds


def strip_pp(s):
    return "\n".join(l for l in s.splitlines() if not l.strip().startswith("#"))


def split_top(cond, op):
    parts = []
    depth = 0
    start = 0
    k = 0
    while k < len(cond):
        ch = cond[k]
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        elif depth == 0 and cond.startswith(op, k):
            parts.append(cond[start:k].strip())
            start = k + len(op)
            k += len(op)
            continue
        k += 1
    parts.append(cond[start:].strip())
    return parts


class Source:
    def __init__(self, root, rel):
        self.rel = rel
        self.path = os.path.join(root, rel)
        with open(self.path, "r", encoding="utf-8-sig", errors="replace") as f:
            self.text = f.read().replace("\r\n", "\n")
        self.lines = self.text.split("\n")
        self.line_starts = [0]
        for ln in self.lines[:-1]:
            self.line_starts.append(self.line_starts[-1] + len(ln) + 1)
        self.lits, braces, self.bounds = scan(self.text)
        self.activity = {name: line_activity(self.lines, m) for name, m in CONFIGS.items()}
        self.blocks = []
        stack = []
        for pos, ch in braces:
            if ch == "{":
                header = self.header_before(pos)
                h = strip_pp(header).strip()
                if re.search(r"\bnamespace\b[^;{}]*$", h) or re.search(r'extern\s+"C"\s*$', h):
                    kind = "namespace"
                elif re.search(r"\b(class|struct|union|enum)\b[^;{}()]*$", h):
                    kind = "class"
                else:
                    kind = "code"
                stack.append([pos, None, kind, h])
            elif stack:
                b = stack.pop()
                b[1] = pos
                self.blocks.append(tuple(b))
        for b in stack:
            self.blocks.append((b[0], len(self.text), b[2], b[3]))

    def header_before(self, pos):
        k = bisect.bisect_left(self.bounds, pos)
        start = self.bounds[k - 1] + 1 if k > 0 else 0
        return self.text[start:pos]

    def line_of(self, pos):
        return bisect.bisect_right(self.line_starts, pos) - 1

    def live(self, pos, config):
        return self.activity[config][self.line_of(pos)]

    def enclosing(self, pos):
        return sorted([b for b in self.blocks if b[0] < pos < b[1]], key=lambda b: b[0])

    def prefix(self, pos):
        k = bisect.bisect_left(self.bounds, pos)
        start = self.bounds[k - 1] + 1 if k > 0 else 0
        return self.text[start:pos]


def is_function_header(h):
    h = h.strip()
    if re.match(r"^(?:else\s+)?(?:if|for|while|switch)\b", h) or h in ("else", "do", "try") or h.endswith("="):
        return False
    return bool(re.search(r"\)\s*(?:const|override|final|noexcept|mutable|\s)*(?:->\s*[\w:<>*& ]+)?$", h))


def gate_guard(src, pos):
    enc = src.enclosing(pos)
    code = [b for b in enc if b[2] == "code"]
    if not code:
        return "static"
    for b in reversed(code):
        m = re.match(r"^(?:else\s+)?if\s*\((.*)\)\s*$", b[3].strip(), re.S)
        if m:
            cond = m.group(1).strip()
            if len(split_top(cond, "||")) == 1 and GATE_CALL in [t.strip() for t in split_top(cond, "&&")]:
                return "guarded:if"
    funcs = [b for b in code if is_function_header(b[3])]
    if funcs:
        fb = funcs[-1]
        body = src.text[fb[0] + 1:pos]
        for m in re.finditer(r"\bif\s*\(([^;{}]*)\)\s*\{?\s*return\b", body):
            at = fb[0] + 1 + m.start()
            inner = [b for b in src.enclosing(at) if b[0] > fb[0]]
            if inner:
                continue
            cond = m.group(1).strip()
            terms = [t.strip() for t in split_top(cond, "||")]
            if len(split_top(cond, "&&")) == 1 and ("!" + GATE_CALL) in terms:
                return "guarded:early_return"
    return None


def module_of(rel):
    parts = rel.replace("\\", "/").split("/")
    return parts[1] if len(parts) > 2 and parts[0] == "Source" else None


def load_modules(root):
    with open(os.path.join(root, "AnomalyInjector.uplugin"), "r", encoding="utf-8-sig") as f:
        desc = json.load(f)
    mods = {}
    for m in desc.get("Modules", []):
        deny = m.get("TargetConfigurationDenyList", m.get("BlacklistTargetConfigurations", []))
        allow = m.get("TargetConfigurationAllowList", m.get("WhitelistTargetConfigurations", None))
        in_ship = "Shipping" not in deny and (allow is None or "Shipping" in allow)
        mods[m["Name"]] = {"shipping": in_ship}
    return mods


def audit(root, binaries=None, binary_config="Development", out=print):
    fails = []

    def fail(rule, msg):
        fails.append(rule)
        out("G-LEVER-AUDIT FAIL %s %s" % (rule, msg))

    try:
        mods = load_modules(root)
    except Exception as e:
        out("G-LEVER-AUDIT REFUSED cannot read AnomalyInjector.uplugin under %s: %s" % (root, e))
        return 2
    srcdir = os.path.join(root, "Source")
    if not os.path.isdir(srcdir):
        out("G-LEVER-AUDIT REFUSED no Source/ under %s" % root)
        return 2

    for mod, key in (("AnomalyCapture", "ANOMALY_CAPTURE"), ("AnomalyControlServer", "ANOMALY_CONTROL_SERVER")):
        bcs = os.path.join(srcdir, mod, mod + ".Build.cs")
        t = open(bcs, "r", encoding="utf-8-sig").read() if os.path.isfile(bcs) else ""
        if not ((key + "=0") in t and (key + "=1") in t and "UnrealTargetConfiguration.Shipping" in t):
            fail("R1", "cannot establish %s per configuration from %s" % (key, os.path.relpath(bcs, root)))

    sources = []
    for base, _dirs, files in os.walk(srcdir):
        for fn in sorted(files):
            if fn.endswith((".cpp", ".h")):
                sources.append(Source(root, os.path.relpath(os.path.join(base, fn), root)))
    sources.sort(key=lambda s: s.rel)
    out("G-LEVER-AUDIT source=%s files=%d modules=%s" % (root, len(sources),
        ",".join("%s(%s)" % (k, "shipping" if v["shipping"] else "no-shipping") for k, v in mods.items())))

    names_live = {c: set() for c in CONFIGS}
    reg_names = {c: set() for c in CONFIGS}
    n_reg = n_static = n_runtime = n_lookup = n_strings = 0

    for s in sources:
        mod = module_of(s.rel)
        mod_ship = mods.get(mod, {"shipping": True})["shipping"]
        is_gate_impl = os.path.normcase(s.rel) == os.path.normcase(GATE_IMPL)
        for (a, b, val) in s.lits:
            if PREFIX not in val:
                continue
            line = s.line_of(a) + 1
            names = NAME_RX.findall(val)
            live = {c: s.live(a, c) for c in CONFIGS}
            for c in CONFIGS:
                if live[c] is not False and not (c == "Shipping" and not mod_ship):
                    names_live[c].update(names)
            ship_live = live["Shipping"] if mod_ship else False
            where = "%s:%d" % (s.rel.replace("\\", "/"), line)
            if ship_live is not False and not (is_gate_impl and val == PREFIX):
                fail("R1", "%s '%s' is %s in a Shipping compile of module %s" % (
                    where, val[:60], "LIVE" if ship_live else "UNDECIDABLE", mod))
            pre = s.prefix(a)
            dev_live = live["Development"] is not False or live["Editor"] is not False
            if REG_RX.search(pre) and names and val == names[0]:
                n_reg += 1
                g = gate_guard(s, a)
                scope = "static" if g == "static" else "runtime"
                if scope == "static":
                    n_static += 1
                else:
                    n_runtime += 1
                for c in CONFIGS:
                    if live[c] is not False:
                        reg_names[c].add(names[0])
                verdict = "OK"
                if dev_live and g is None:
                    verdict = "FAIL"
                    fail("R2", "%s %s is a RUNTIME registration not reached through %s" % (where, names[0], GATE_CALL))
                out("G-LEVER-AUDIT REG %-38s %-62s module=%s scope=%s shipping=%s gate=%s -> %s" % (
                    names[0], where, mod, scope, "excluded" if ship_live is False else "LIVE",
                    "swept" if g == "static" else (g or "NONE"), verdict))
                continue
            cm = CALL_RX.search(pre)
            if cm and names and val == names[0] and (cm.group(1) in RAW_LOOKUPS or cm.group(1) in ALLOWED_LOOKUPS):
                n_lookup += 1
                fn = cm.group(1)
                ok = fn in ALLOWED_LOOKUPS
                out("G-LEVER-AUDIT LOOKUP %-35s %-62s via=%s -> %s" % (names[0], where, fn, "OK" if ok else "FAIL"))
                if not ok:
                    fail("R5", "%s looks %s up with %s, which does not honour the mask" % (where, names[0], fn))
                continue
            n_strings += 1

    bench_classes = set()
    for s in sources:
        if "Anomalies" in s.rel and "IAIBenchFixture" in s.text:
            bench_classes.update(re.findall(r"\b(FAnomaly_\w+)::\w+\s*\(", s.text))
    ais = [s for s in sources if s.rel.replace("\\", "/").endswith("AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp")]
    n_cat = n_bench_cat = 0
    if not ais:
        fail("R4", "AnomalyInjectorSubsystem.cpp not found")
    else:
        s = ais[0]
        for m in re.finditer(r"Register\s*\(\s*MakeUnique\s*<\s*(\w+)\s*>\s*\(([^;]*?)\)\s*\)\s*;", s.text):
            n_cat += 1
            cls = m.group(1)
            idm = re.search(r'FName\s*\(\s*TEXT\s*\(\s*"([^"]+)"', m.group(2))
            cid = idm.group(1) if idm else "-"
            bench = cls in bench_classes or cid in TWIN_IDS
            where = "%s:%d" % (s.rel.replace("\\", "/"), s.line_of(m.start()) + 1)
            ship = s.live(m.start(), "Shipping")
            g = gate_guard(s, m.start())
            verdict = "OK"
            if bench:
                n_bench_cat += 1
                if ship is not False:
                    verdict = "FAIL"
                    fail("R4", "%s bench catalogue entry %s id=%s is compiled into Shipping" % (where, cls, cid))
                if g != "guarded:if":
                    verdict = "FAIL"
                    fail("R4", "%s bench catalogue entry %s id=%s registers outside %s" % (where, cls, cid, GATE_CALL))
            out("G-LEVER-AUDIT CATALOG %-24s id=%-12s %-62s bench=%s shipping=%s gate=%s -> %s" % (
                cls, cid, where, "yes" if bench else "no", "excluded" if ship is False else "live",
                g if bench else "n/a", verdict))
        if n_bench_cat == 0:
            fail("R4", "no bench catalogue entry found - the rule has nothing to check (classes requiring -IAIBenchFixture: %s)"
                 % (",".join(sorted(bench_classes)) or "none"))
    pool_hits = 0
    for s in sources:
        r = s.rel.replace("\\", "/")
        if r.endswith("AnomalyAutoInjectorSubsystem.cpp") or r.endswith("AnomalySelectorSubsystem.cpp"):
            for tid in TWIN_IDS:
                if ('"%s"' % tid) in s.text:
                    pool_hits += 1
                    fail("R4", "%s names bench twin '%s' (a pool or selector list must not)" % (r, tid))
    out("G-LEVER-AUDIT POOL twins named in the auto pool or the selector: %d -> %s" % (pool_hits, "OK" if not pool_hits else "FAIL"))

    def unconditional_call(src, needle, func_rx):
        for m in re.finditer(re.escape(needle), src.text):
            if src.live(m.start(), "Development") is not True:
                continue
            code = [b for b in src.enclosing(m.start()) if b[2] == "code"]
            if len(code) >= 1 and re.search(func_rx, code[0][3]) and all(
                    is_function_header(b[3]) and not re.match(r"^(?:else\s+)?if\b", b[3].strip()) for b in code):
                return True
        return False

    ok3 = True
    if not ais or not unconditional_call(ais[0], SWEEP_CALL, r"UAnomalyInjectorSubsystem::Initialize\s*\("):
        ok3 = False
        fail("R3", "UAnomalyInjectorSubsystem::Initialize does not call %s unconditionally" % SWEEP_CALL)
    mod_cpp = [s for s in sources if s.rel.replace("\\", "/").endswith("AnomalyInjector/Private/AnomalyInjectorModule.cpp")]
    if not mod_cpp or "OnPostEngineInit" not in mod_cpp[0].text or not unconditional_call(mod_cpp[0], SWEEP_CALL, r"StartupModule\s*\("):
        ok3 = False
        fail("R3", "AnomalyInjectorModule.cpp does not sweep at OnPostEngineInit")
    gate = [s for s in sources if os.path.normcase(s.rel) == os.path.normcase(GATE_IMPL)]
    if not gate:
        ok3 = False
        fail("R3", "%s is missing" % GATE_IMPL)
    else:
        g = gate[0].text
        body = re.search(r"bool\s+IsEnabled\s*\(\s*\)\s*\{(.*?)\n\t\}", g, re.S)
        b = body.group(1) if body else ""
        if not ('TEXT("IAIBench")' in b and 'TEXT("IAIBenchFixture")' in b and "UE_BUILD_SHIPPING" in b and "return false" in b):
            ok3 = False
            fail("R3", "IsEnabled() must open on -IAIBench and -IAIBenchFixture and return false in Shipping")
        if "ECVF_Unregistered" not in g or "ForEachConsoleObjectThatStartsWith" not in g:
            ok3 = False
            fail("R3", "the sweep no longer masks by prefix with ECVF_Unregistered")
    out("G-LEVER-AUDIT SWEEP initialize+post_engine_init gate=-IAIBench|-IAIBenchFixture -> %s" % ("OK" if ok3 else "FAIL"))

    for s in sources:
        if os.path.normcase(s.rel) == os.path.normcase(GATE_IMPL):
            continue
        for m in re.finditer(r'TEXT\s*\(\s*"IAIBench"\s*\)', s.text):
            fail("R6", "%s:%d tests -IAIBench inline; use %s" % (s.rel.replace("\\", "/"), s.line_of(m.start()) + 1, GATE_CALL))

    if binaries:
        blob = b""
        for p in binaries:
            with open(p, "rb") as f:
                blob += f.read() + b"\0"
        found = set()
        for enc, rx in (("ascii", re.compile(rb"IAI\.Bench\.[A-Za-z0-9_]+")),
                        ("utf16", re.compile(rb"I\0A\0I\0\.\0B\0e\0n\0c\0h\0\.\0(?:[A-Za-z0-9_]\0)+"))):
            for m in rx.finditer(blob):
                found.add(m.group(0).decode("ascii") if enc == "ascii" else m.group(0).decode("utf-16-le"))
        src_all = names_live[binary_config]
        src_reg = reg_names[binary_config]
        extra = sorted(found - src_all)
        missing = sorted(src_reg - found)
        echo_missing = [e for e in ECHO_STRINGS if e.encode("utf-16-le") not in blob and e.encode("ascii") not in blob]
        out("G-LEVER-AUDIT BINARY config=%s files=%d names_in_binary=%d names_in_source=%d registered=%d unknown=%d missing=%d echo=%s" % (
            binary_config, len(binaries), len(found), len(src_all), len(src_reg), len(extra), len(missing),
            "present" if not echo_missing else "MISSING " + ",".join(echo_missing)))
        for x in extra:
            fail("B1", "binary carries %s, which this source does not compile in %s" % (x, binary_config))
        for x in missing:
            fail("B2", "source registers %s in %s but the binary does not carry it (stale build)" % (x, binary_config))
        if echo_missing:
            fail("B3", "gate echo string(s) absent from the binary: %s" % ", ".join(echo_missing))

    out("G-LEVER-AUDIT SUMMARY lever_names=%d registrations=%d static=%d runtime=%d lookups=%d other_strings=%d catalog=%d bench_catalog=%d failures=%d" % (
        len(names_live["Development"] | names_live["Editor"]), n_reg, n_static, n_runtime, n_lookup, n_strings,
        n_cat, n_bench_cat, len(fails)))
    out("G-LEVER-AUDIT VERDICT %s%s" % ("PASS" if not fails else "FAIL", "" if not fails else " rules=" + ",".join(sorted(set(fails)))))
    return 0 if not fails else 1


AIS = "Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp"
MUTANTS = [
    ("R1", "a static lever appended to a Shipping-compiled file", "Source/AnomalyInjector/Private/AnomalyViewport.cpp",
     r"\Z", '\nstatic FAutoConsoleCommand GPlantedLever(TEXT("IAI.Bench.PlantedLever"), TEXT("planted"), '
            'FConsoleCommandDelegate::CreateLambda([](){}));\n'),
    ("R1", "the AIS lever block's Shipping exclusion removed", AIS,
     r"#if !UE_BUILD_SHIPPING(\r?\n)static FAutoConsoleCommandWithWorldAndArgs GSynthTickOrderCmd\(",
     r"#if 1\1static FAutoConsoleCommandWithWorldAndArgs GSynthTickOrderCmd("),
    ("R2", "a runtime lever registered in Initialize without the gate", AIS,
     r'(AnomalyBenchGate::SweepUngatedLevers\(TEXT\("subsystem_init"\)\);)',
     r'\1\n#if !UE_BUILD_SHIPPING\n\tIConsoleManager::Get().RegisterConsoleCommand(TEXT("IAI.Bench.PlantedRuntime"), '
     r'TEXT("planted"), FConsoleCommandDelegate::CreateLambda([](){}), ECVF_Default);\n#endif'),
    ("R2", "the bench module registers its commands without the gate", "Source/AnomalyBench/Private/AnomalyBenchModule.cpp",
     r"if \(AnomalyBenchGate::IsEnabled\(\)\)(\r?\n\t\t\{\r?\n\t\t\tPlace =)", r"if (true)\1"),
    ("R3", "the sweep call removed from Initialize", AIS,
     r'\tAnomalyBenchGate::SweepUngatedLevers\(TEXT\("subsystem_init"\)\);\r?\n', ""),
    ("R3", "IsEnabled() no longer opens on -IAIBenchFixture", GATE_IMPL.replace("\\", "/"),
     r' \|\| FParse::Param\(CommandLine, TEXT\("IAIBenchFixture"\)\);', ";"),
    ("R4", "the twins registered outside the gate", AIS,
     r"if \(AnomalyBenchGate::IsEnabled\(\)\)(\r?\n\t\{\r?\n\t\tRegister\(MakeUnique<FAnomaly_ChangeCase>)", r"if (true)\1"),
    ("R4", "the twins compiled into Shipping", AIS,
     r"#if !UE_BUILD_SHIPPING(\r?\n\tif \(AnomalyBenchGate::IsEnabled\(\)\)\r?\n\t\{\r?\n\t\tRegister\(MakeUnique<FAnomaly_ChangeCase>)",
     r"#if 1\1"),
    ("R5", "a lever looked up with a raw FindConsoleObject", "Source/AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp",
     r'AnomalyBenchGate::FindLeverCommand\(TEXT\("IAI\.Bench\.PlaceView"\)\)',
     r'IConsoleManager::Get().FindConsoleObject(TEXT("IAI.Bench.PlaceView"))->AsCommand()'),
    ("R6", "a second, inline -IAIBench gate", "Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp",
     r"else if \(Auto && AnomalyBenchGate::IsEnabled\(\)\)",
     r'else if (Auto && FParse::Param(FCommandLine::Get(), TEXT("IAIBench")))'),
]


def selftest(root):
    tmp = tempfile.mkdtemp(prefix="lever_audit_")
    results = []
    try:
        def fresh():
            d = os.path.join(tmp, "tree")
            if os.path.isdir(d):
                shutil.rmtree(d)
            shutil.copytree(os.path.join(root, "Source"), os.path.join(d, "Source"))
            shutil.copy(os.path.join(root, "AnomalyInjector.uplugin"), d)
            return d

        sink = []
        d = fresh()
        rc = audit(d, out=sink.append)
        ok = rc == 0
        results.append(ok)
        print("G-LEVER-AUDIT SELFTEST control (clean copy)                         -> %s (exit %d)" % ("PASS as expected" if ok else "*** DID NOT PASS ***", rc))
        if not ok:
            for l in sink:
                if " FAIL " in l:
                    print("    " + l)
        def admit_shipping(d, module):
            dp = os.path.join(d, "AnomalyInjector.uplugin")
            with open(dp, "r", encoding="utf-8-sig") as f:
                desc = json.load(f)
            changed = False
            for m in desc.get("Modules", []):
                if m.get("Name") != module:
                    continue
                for key in ("TargetConfigurationDenyList", "BlacklistTargetConfigurations"):
                    if "Shipping" in m.get(key, []):
                        m[key] = [x for x in m[key] if x != "Shipping"]
                        changed = True
            with open(dp, "w", encoding="utf-8") as f:
                json.dump(desc, f, indent="\t")
            return changed

        cases = [(r, l, rel, pat, rep, r == "R1") for r, l, rel, pat, rep in MUTANTS]
        cases.append(("R1", "the first R1 lever in a module Shipping does not build", MUTANTS[0][2], MUTANTS[0][3],
                      MUTANTS[0][4], False))
        for rule, label, rel, pat, rep, admit in cases:
            d = fresh()
            p = os.path.join(d, rel.replace("/", os.sep))
            with open(p, "r", encoding="utf-8-sig", errors="replace", newline="") as f:
                t = f.read()
            t2, k = re.subn(pat, rep, t, count=1)
            if k != 1:
                results.append(False)
                print("G-LEVER-AUDIT SELFTEST %s %-52s -> *** ANCHOR NOT FOUND (mutant not planted) ***" % (rule, label))
                continue
            with open(p, "w", encoding="utf-8", newline="") as f:
                f.write(t2)
            excluded = rule == "R1" and not admit and not load_modules(d)[module_of(rel)]["shipping"]
            if admit:
                admit_shipping(d, module_of(rel))
            sink = []
            rc = audit(d, out=sink.append)
            rules = set(re.findall(r"G-LEVER-AUDIT FAIL (\w+) ", "\n".join(sink)))
            if rule == "R1" and not admit:
                ok = excluded and rc == 0
                verdict = "PASSED as expected (the descriptor keeps the module out of Shipping)" if ok else "*** WRONG ***"
            else:
                ok = rc == 1 and rule in rules
                verdict = "FAILED as expected" if ok else "*** NOT CAUGHT ***"
            results.append(ok)
            first = next((l for l in sink if (" FAIL %s " % rule) in l), "")
            print("G-LEVER-AUDIT SELFTEST %s %-52s -> %s (exit %d, rules %s)%s" % (
                rule, label, verdict, rc, ",".join(sorted(rules)) or "none",
                " [module re-admitted to Shipping in the copy]" if admit else ""))
            if first:
                print("    " + first.replace("G-LEVER-AUDIT ", "")[:180])
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    passed = all(results)
    print("G-LEVER-AUDIT SELFTEST %d case(s): %s" % (len(results), "OK" if passed else "FAILED"))
    return 0 if passed else 1


def main():
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    ap = argparse.ArgumentParser(description="G-LEVER-AUDIT: bench levers and bench catalogue entries behind the bench gate.")
    ap.add_argument("--root", default=here, help="plugin root (holds AnomalyInjector.uplugin and Source/)")
    ap.add_argument("--binary", action="append", default=[], help="built exe/dll to cross-check (repeatable; union)")
    ap.add_argument("--binary-config", default="Development", choices=sorted(CONFIGS))
    ap.add_argument("--selftest", action="store_true", help="plant one violation per rule in a copy; each must FAIL")
    a = ap.parse_args()
    if a.selftest:
        return selftest(a.root)
    return audit(a.root, a.binary, a.binary_config)


if __name__ == "__main__":
    sys.exit(main())
