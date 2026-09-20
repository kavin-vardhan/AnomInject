"""m52 planning probe: how widely is a candidate target's texture SHARED?

EVIDENCE, NOT A MEASUREMENT - the confidence claim travels with the tool.
Read-only. Opens no engine, builds nothing, runs no capture leg.

WHY IT EXISTS
-------------
m52 ("stuck_low_mip") holds a texture at a low resident mip. A texture is an
ASSET, so every component sampling it goes blurry, while the label names ONE
target. Before choosing a sharing policy we need to know how often a candidate
target's texture is reachable from some OTHER mesh in the same project.

WHAT IT READS
-------------
An uncooked .uasset stores its name table in the package header, at the front of
the file. Object paths that the asset references ("/Game/...", "/Engine/...")
appear there as name entries. This tool reads the first HEAD_BYTES of every
.uasset, pulls out those paths, and builds a reference graph:

    mesh .uasset  --refs-->  material .uasset  --refs-->  texture .uasset

It then computes, for each asset, the textures reachable in <= 2 hops
(TexReach), inverts that map, and reports for each named target how many OTHER
assets reach the same texture.

TEXTURE CLASSIFIER
------------------
An asset is called a Texture2D iff its head contains the token "Texture2D" AND
at least one texture-only serialised property name ("MipGenSettings",
"CompressionSettings", "AddressX", "PowerOfTwoMode"). The conjunction is needed
because a MATERIAL also carries "Texture2D" - as the class name of its texture
IMPORTS. The tool prints the classifier's own both-ways control (how many
"Texture2D"-containing assets it accepted and rejected) so the split is
auditable rather than assumed.

STATED WEAKNESSES (do not drop these when quoting a result)
-----------------------------------------------------------
1. LEVEL-WIDE, NOT VIEW-WIDE. It counts assets that reach a texture anywhere in
   the project. The question m52 actually needs - "how many OTHER actors VISIBLE
   IN THIS FRAME use it" - needs a runtime probe against the visible set. This
   number is therefore an UPPER BOUND on sharing and a LOWER BOUND on
   exclusivity.
2. ASSET-WIDE, NOT INSTANCE-WIDE. One shared mesh placed 50 times counts once.
3. Dynamic material instances created at runtime are invisible to it.
4. A path string appearing in a head region is a reference, not proof the
   texture is sampled by the shader that draws the target.
5. HEAD_BYTES truncation: an asset whose name table runs past HEAD_BYTES loses
   its tail references. The tool reports how many files were truncated.
6. A MESH ASSET'S DEFAULT MATERIAL SLOT IS NOT WHAT A LEVEL NECESSARILY DRAWS.
   A placed component can override the slot, and then the mesh asset's own
   material - and every texture reached through it - is not in the picture at
   all. MEASURED INSTANCE (2026-09-20): this tool reports /Engine/BasicShapes/Cube
   reaching T_Default_Material_Grid_M/N with 25 other-mesh sharers, and on
   CB_GateLevel that is simply not true - make_gate_level.py:60,68-69 assigns
   /Engine/BasicShapes/BasicShapeMaterial to every target, and that material
   references no texture at all. The scan describes ASSETS; a level describes
   INSTANCES. Check what the level assigns before quoting a row about a fixture.

Because of (1)-(6) this is load-bearing only as a DIFFERENTIAL between fixtures
and between targets, never as an absolute incidence claim.

Usage:
    python m52_texture_sharing_scan.py --root <content-dir> [--root ...]
                                       [--target <AssetName>]... [--top N]
"""

import argparse
import os
import re
import sys
from collections import defaultdict

HEAD_BYTES = 192 * 1024

PATH_RE = re.compile(rb"/(?:Game|Engine)/[A-Za-z0-9_/]+(?:\.[A-Za-z0-9_]+)?")

TEXTURE_CLASS_TOKEN = b"Texture2D"
TEXTURE_ONLY_PROPS = (b"MipGenSettings", b"CompressionSettings", b"AddressX", b"PowerOfTwoMode")


def object_path_to_key(raw):
    """/Game/A/B.B  ->  /Game/A/B   (package path, which is what a file maps to)."""
    s = raw.decode("ascii", "ignore")
    if "." in s:
        s = s.split(".", 1)[0]
    return s


def file_to_key(path, root, mount):
    rel = os.path.relpath(path, root).replace("\\", "/")
    if rel.lower().endswith(".uasset"):
        rel = rel[: -len(".uasset")]
    return mount + "/" + rel


def scan_file(path):
    try:
        with open(path, "rb") as f:
            head = f.read(HEAD_BYTES)
    except OSError:
        return None
    truncated = len(head) == HEAD_BYTES
    is_tex = TEXTURE_CLASS_TOKEN in head and any(p in head for p in TEXTURE_ONLY_PROPS)
    has_tex_token = TEXTURE_CLASS_TOKEN in head
    refs = set()
    for m in PATH_RE.finditer(head):
        refs.add(object_path_to_key(m.group(0)))
    return refs, is_tex, has_tex_token, truncated


def index_roots(roots):
    assets = {}
    stats = {"files": 0, "truncated": 0, "tex": 0, "tex_token_only": 0}
    for root, mount in roots:
        if not os.path.isdir(root):
            print("  [skip] missing root %s" % root)
            continue
        for dirpath, _dirs, files in os.walk(root):
            for n in files:
                if not n.lower().endswith(".uasset"):
                    continue
                p = os.path.join(dirpath, n)
                r = scan_file(p)
                if r is None:
                    continue
                refs, is_tex, has_tex_token, truncated = r
                key = file_to_key(p, root, mount)
                assets[key] = {"refs": refs, "tex": is_tex, "file": p,
                               "name": os.path.splitext(n)[0]}
                stats["files"] += 1
                stats["truncated"] += 1 if truncated else 0
                stats["tex"] += 1 if is_tex else 0
                if has_tex_token and not is_tex:
                    stats["tex_token_only"] += 1
    return assets, stats


def tex_reach(assets):
    """Textures reachable from each asset within two reference hops.

    Returns (reach, direct). 'direct' is the 1-hop set: an asset that names a
    texture directly is a MATERIAL. The 2-hop-only set is what a MESH looks
    like. The split matters: counting a target's own master material and its
    instances as "sharers" would report every target as shared.
    """
    reach = {}
    direct = {}
    for key, a in assets.items():
        found = set()
        one_hop = set()
        for r in a["refs"]:
            ra = assets.get(r)
            if ra is None:
                continue
            if ra["tex"]:
                found.add(r)
                one_hop.add(r)
                continue
            for r2 in ra["refs"]:
                r2a = assets.get(r2)
                if r2a is not None and r2a["tex"]:
                    found.add(r2)
        reach[key] = found
        direct[key] = one_hop
    return reach, direct


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", action="append", required=True,
                    help="<dir>=<mount>, e.g. D:\\proj\\Content=/Game")
    ap.add_argument("--target", action="append", default=[],
                    help="asset NAME (not path) of a measured candidate target")
    ap.add_argument("--top", type=int, default=12)
    args = ap.parse_args()

    roots = []
    for spec in args.root:
        if "=" in spec:
            d, mount = spec.rsplit("=", 1)
        else:
            d, mount = spec, "/Game"
        roots.append((d, mount))

    print("m52 texture-sharing scan - EVIDENCE, not a measurement (see module docstring)")
    print("roots: %s" % ", ".join("%s -> %s" % (d, m) for d, m in roots))
    assets, stats = index_roots(roots)
    print("indexed %d .uasset  |  classified texture: %d  |  carried the Texture2D token but "
          "was REJECTED by the property conjunction: %d  |  head-truncated: %d"
          % (stats["files"], stats["tex"], stats["tex_token_only"], stats["truncated"]))
    print("  (a non-zero REJECTED count is the classifier's both-ways control: it proves the")
    print("   conjunction discriminates rather than accepting everything that mentions a texture)")

    reach, direct = tex_reach(assets)
    mesh_sharers = defaultdict(set)
    for key, texs in reach.items():
        one_hop = direct[key]
        for t in texs:
            if t not in one_hop:
                mesh_sharers[t].add(key)

    by_name = defaultdict(list)
    for key, a in assets.items():
        by_name[a["name"]].append(key)

    print()
    print("%-24s %-46s %5s %5s  %s"
          % ("target asset", "resolved package", "#tex", "excl", "per-texture OTHER-MESH sharer count"))
    print("-" * 138)

    exclusive = 0
    has_one_exclusive = 0
    considered = 0
    unresolved = []
    for t in args.target:
        keys = by_name.get(t)
        if not keys:
            unresolved.append(t)
            print("%-24s %-46s %5s %5s  %s" % (t, "(NOT FOUND in indexed roots)", "-", "-", "-"))
            continue
        for key in sorted(keys):
            texs = sorted(reach.get(key, ()))
            counts = []
            n_excl = 0
            for tx in texs:
                others = mesh_sharers[tx] - {key}
                counts.append("%s=%d" % (tx.rsplit("/", 1)[-1], len(others)))
                if len(others) == 0:
                    n_excl += 1
            considered += 1
            if texs and n_excl == len(texs):
                exclusive += 1
            if n_excl > 0:
                has_one_exclusive += 1
            print("%-24s %-46s %5d %5d  %s"
                  % (t, key, len(texs), n_excl,
                     ", ".join(counts[: args.top]) + (" ..." if len(counts) > args.top else "")
                     if counts else "(no texture reached)"))

    print()
    if considered:
        print("ALL reached textures exclusive to this mesh (level-wide): %d of %d (%.1f%%)"
              % (exclusive, considered, 100.0 * exclusive / considered))
        print("AT LEAST ONE exclusive texture available to pick: %d of %d (%.1f%%)"
              % (has_one_exclusive, considered, 100.0 * has_one_exclusive / considered))
    if unresolved:
        print("UNRESOLVED target names (not in these roots): %s" % ", ".join(unresolved))
    print()
    print("READ THIS WITH THE RESULT: sharer counts are LEVEL-WIDE, not view-wide, and")
    print("ASSET-WIDE, not instance-wide. They bound the hazard from above; they do not")
    print("measure how many OTHER actors were visible in any particular frame.")


if __name__ == "__main__":
    sys.exit(main())
