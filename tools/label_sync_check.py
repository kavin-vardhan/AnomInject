"""label_sync_check.py - the office label-sync check. It prints numbers only.

  python label_sync_check.py --selftest
  python label_sync_check.py <session folder, or a folder of sessions> [more folders] [--out numbers.txt]

It reads capture sessions without changing them and prints, for each anomaly type, how many frames each label
edge is off from the picture: the first and the last frame on which the anomaly is visible, measured from the
saved frames themselves. No folder, object or frame names are printed, so the result can be read aloud.

Standard library only (Python 3.8 or newer). When Pillow is installed it is used to decode PNGs faster; both
decoders give identical numbers, which --selftest proves. --stdlib forces the standard-library decoder.
Exit codes: 0 read, 1 selftest failed, 2 nothing readable.
"""
import argparse
import collections
import itertools
import json
import math
import operator
import os
import re
import shutil
import struct
import sys
import tempfile
import time
import zlib

KIT_VERSION = "1.0"
EVALUATOR = "087-01"
METHOD = ("086-01 per-frame change on the target silhouette; 084-06 edge-local references, stuck_low_mip "
          "sharpness path and transition-aware gate; 084-07 labelled and partial rules")

TYPE_MAP = {"blink": "blinking", "flicker": "blinking"}
DELIVERED = ("blinking", "missing_object", "missing_texture", "corrupted_texture", "lod_popping",
             "stuck_low_mip", "uv_corruption", "normal_corruption", "camera_clipping")
M52 = "stuck_low_mip"
HIDE = ("blinking", "missing_object")
NOT_JUDGEABLE = {
    "camera_clipping": "no target mask, and its label is a whole-frame proxy: judging it needs a matched null capture",
    "time_dilation": "it changes timing, not the target's pixels",
    "lighting_mismatch": "its effect is the scene's lighting, not the target's own pixels",
}
AA_ONLY_REASONS = ("temporal_aa", "hide_return")

MOVE_CM = 0.5
MOVE_DEG = 0.05
REF_BACK = 12
SPAN_PAD = 16
SETTLE_SKIP = 30
ROI_DILATE = 4
OUT_DILATE = 24
K_SIG = 6.0
D_FLOOR = 1.0
P_FLOOR = 0.01
P_PIX = 8
REL = 0.10
OUT_PIX = 48
OUT_BLOB_MIN = 400
MEAS_P = 0.20
MEAS_K = 3.0
SUFFIX = 3
DS = 4
POST_REF_N = 6
REF_MIN = 4
PREV_SETTLE = 4
POST_SKIP = 2

PRE_W = 12
PRE_N = 8
SPAN_AFTER = 60
POST_N = 6
K_SIGMA = 5.0
REL_FLOOR = 0.02
DEPTH_K = 3.0
NOISE_WIN = 40
ONSET_CONFIRM = 2
DECAY_K = 3.0
ROI_SHRINK = 0.15
PARTIAL_INVESTIGATE = 8

THRESH = (("strict", 0.0), ("t10", 0.10), ("t50", 0.50))
RELEASE = "t50"

_sub = operator.sub
_add = operator.add
_AND255 = (255).__and__
_LE8 = bytes(range(P_PIX + 1))
_LE48 = bytes(range(OUT_PIX + 1))
_GT48_ASCII = bytes(49 if i > OUT_PIX else 48 for i in range(256))
_ONE_ASCII = bytes(49 if i else 48 for i in range(256))
_EQ_ASCII = {}
_T299 = [299 * i for i in range(256)]
_T587 = [587 * i for i in range(256)]
_T114 = [114 * i for i in range(256)]


def jload(path, default=None):
    try:
        with open(path, "r", encoding="utf-8-sig") as fh:
            return json.load(fh)
    except Exception:
        return default


def read_jsonl(path):
    out = []
    try:
        with open(path, "r", encoding="utf-8-sig") as fh:
            for line in fh:
                line = line.strip()
                if line:
                    try:
                        out.append(json.loads(line))
                    except ValueError:
                        pass
    except OSError:
        pass
    return out


def runs_of(values):
    out = []
    for x in sorted(values):
        if out and x == out[-1][1] + 1:
            out[-1][1] = x
        else:
            out.append([x, x])
    return out


def median(values):
    v = sorted(values)
    n = len(v)
    if not n:
        return 0.0
    if n % 2:
        return float(v[n // 2])
    return (v[n // 2 - 1] + v[n // 2]) / 2.0


def pstdev(values):
    n = len(values)
    if not n:
        return 0.0
    m = sum(values) / n
    return math.sqrt(sum((x - m) * (x - m) for x in values) / n)


def robust_sigma(values):
    if not values:
        return 0.0
    m = median(values)
    return 1.4826 * median([abs(x - m) for x in values])


def rolling_median(values, w=9):
    h = w // 2
    n = len(values)
    return [median(values[max(0, i - h):i + h + 1]) for i in range(n)]


def popcount(v):
    return bin(v).count("1")


def bit_runs(v):
    s = bin(v)[2:][::-1]
    return [(m.start(), m.end()) for m in re.finditer("1+", s)]


def hist(values):
    c = collections.Counter(values)
    if not values:
        return "{}"
    return "{" + ", ".join("%+d:%d" % (k, c[k]) for k in sorted(c)) + "}"


class PngError(Exception):
    pass


PNG_SIG = b"\x89PNG\r\n\x1a\n"
CHANNELS = {0: 1, 2: 3, 3: 1, 4: 2, 6: 4}
_PIL = None
_PIL_TRIED = False


def pillow():
    global _PIL, _PIL_TRIED
    if not _PIL_TRIED:
        _PIL_TRIED = True
        try:
            from PIL import Image
            _PIL = Image
        except Exception:
            _PIL = None
    return _PIL


def png_parse(path, need_data=True):
    with open(path, "rb") as fh:
        data = fh.read() if need_data else fh.read(64)
    if data[:8] != PNG_SIG:
        raise PngError("not a PNG")
    pos = 8
    ihdr = None
    plte = None
    idat = []
    n = len(data)
    while pos + 8 <= n:
        length, ctype = struct.unpack(">I4s", data[pos:pos + 8])
        body = data[pos + 8:pos + 8 + length]
        pos += 12 + length
        if ctype == b"IHDR":
            ihdr = struct.unpack(">IIBBBBB", body[:13])
            if not need_data:
                break
        elif ctype == b"PLTE":
            plte = body
        elif ctype == b"IDAT":
            idat.append(body)
        elif ctype == b"IEND":
            break
    if ihdr is None:
        raise PngError("no IHDR")
    w, h, depth, ctype_, _comp, _filt, inter = ihdr
    if depth != 8 or inter != 0 or ctype_ not in CHANNELS:
        raise PngError("unsupported PNG")
    if need_data and ctype_ == 3 and plte is None:
        raise PngError("palette PNG without a palette")
    return w, h, ctype_, plte, idat


_SWAR = {}


def _swar(nb):
    m = _SWAR.get(nb)
    if m is None:
        m = (int.from_bytes(b"\x7f" * nb, "big"), int.from_bytes(b"\x80" * nb, "big"))
        _SWAR[nb] = m
    return m


def _unfilter(ft, raw, prev, bpp, nb):
    if ft == 0:
        return raw
    if ft == 1:
        out = bytearray(nb)
        for c in range(bpp):
            out[c::bpp] = bytes(map(_AND255, itertools.accumulate(raw[c::bpp])))
        return bytes(out)
    if ft == 2:
        m7, m8 = _swar(nb)
        a = int.from_bytes(raw, "big")
        b = int.from_bytes(prev, "big")
        return (((a & m7) + (b & m7)) ^ ((a ^ b) & m8)).to_bytes(nb, "big")
    if ft == 3:
        out = bytearray(nb)
        for i in range(min(bpp, nb)):
            out[i] = (raw[i] + (prev[i] >> 1)) & 255
        for i in range(bpp, nb):
            out[i] = (raw[i] + ((out[i - bpp] + prev[i]) >> 1)) & 255
        return bytes(out)
    if ft == 4:
        out = bytearray(nb)
        for i in range(min(bpp, nb)):
            out[i] = (raw[i] + prev[i]) & 255
        for i in range(bpp, nb):
            a = out[i - bpp]
            b = prev[i]
            c = prev[i - bpp]
            pa = b - c
            pb = a - c
            pc = pa + pb
            if pa < 0:
                pa = -pa
            if pb < 0:
                pb = -pb
            if pc < 0:
                pc = -pc
            if pa <= pb and pa <= pc:
                out[i] = (raw[i] + a) & 255
            elif pb <= pc:
                out[i] = (raw[i] + b) & 255
            else:
                out[i] = (raw[i] + c) & 255
        return bytes(out)
    raise PngError("bad filter type")


def decode_native(path, decoder, max_rows=None, max_cols=None):
    if decoder == "pillow":
        w, h, ct, _plte, _ = png_parse(path, need_data=False)
        plte = None
        if ct == 3:
            plte = png_parse(path)[3]
        im = pillow().open(path)
        want = {0: "L", 2: "RGB", 3: "P", 4: "LA", 6: "RGBA"}[ct]
        if im.mode != want:
            raise PngError("decoder mode mismatch")
        data = im.tobytes()
        ch = CHANNELS[ct]
        nrows = h if max_rows is None else max(0, min(h, max_rows))
        ncols = w if max_cols is None else max(0, min(w, max_cols))
        rs = w * ch
        nb = ncols * ch
        rows = [data[y * rs:y * rs + nb] for y in range(nrows)]
        return w, h, ct, plte, ncols, rows
    w, h, ct, plte, idat = png_parse(path)
    ch = CHANNELS[ct]
    stride = 1 + w * ch
    nrows = h if max_rows is None else max(0, min(h, max_rows))
    ncols = w if max_cols is None else max(0, min(w, max_cols))
    nb = ncols * ch
    need = nrows * stride
    dobj = zlib.decompressobj()
    buf = dobj.decompress(b"".join(idat), need)
    while len(buf) < need and dobj.unconsumed_tail:
        buf += dobj.decompress(dobj.unconsumed_tail, need - len(buf))
    if len(buf) < need:
        raise PngError("truncated image data")
    rows = []
    prev = bytes(nb)
    for y in range(nrows):
        o = y * stride
        cur = _unfilter(buf[o], buf[o + 1:o + 1 + nb], prev, ch, nb)
        rows.append(cur)
        prev = cur
    return w, h, ct, plte, ncols, rows


def rows_to_rgb(rows, ct, plte, ncols):
    if ct == 2:
        return rows
    out = []
    if ct == 6:
        for r in rows:
            o = bytearray(3 * ncols)
            o[0::3] = r[0::4]
            o[1::3] = r[1::4]
            o[2::3] = r[2::4]
            out.append(bytes(o))
        return out
    if ct in (0, 4):
        for r in rows:
            g = r if ct == 0 else r[0::2]
            o = bytearray(3 * ncols)
            o[0::3] = g
            o[1::3] = g
            o[2::3] = g
            out.append(bytes(o))
        return out
    pal = [plte[3 * i:3 * i + 3] if 3 * i + 3 <= len(plte) else b"\x00\x00\x00" for i in range(256)]
    return [b"".join(map(pal.__getitem__, r)) for r in rows]


def _luma(r, g, b):
    return (r * 19595 + g * 38470 + b * 7471 + 0x8000) >> 16


def rows_to_gray(rows, ct, plte, ncols):
    if ct == 0:
        return rows
    if ct == 4:
        return [r[0::2] for r in rows]
    rgb = rows_to_rgb(rows, ct, plte, ncols)
    return [bytes(map(_luma, r[0::3], r[1::3], r[2::3])) for r in rgb]


class Frame(object):
    __slots__ = ("w", "h", "rows", "ncols")

    def __init__(self, w, h, rows, ncols):
        self.w = w
        self.h = h
        self.rows = rows
        self.ncols = ncols


class Session(object):
    def __init__(self, d, decoder):
        self.dir = d
        self.decoder = decoder
        self.anno = jload(os.path.join(d, "annotation.json"), {}) or {}
        self.rows = {}
        for r in read_jsonl(os.path.join(d, "labels.jsonl")):
            si = r.get("session_index")
            if isinstance(si, int) and si not in self.rows:
                self.rows[si] = r
        self.summary = jload(os.path.join(d, "run_summary.json"), {}) or {}
        self.ce_rows = [r for r in read_jsonl(os.path.join(d, "change_evidence.jsonl")) if r.get("kind") == "pair"]
        self.sis = sorted(self.rows)
        self._full = collections.OrderedDict()
        self._mask = collections.OrderedDict()
        self.decoded = 0
        self.decoded_partial = 0
        self.size = None

    def frame_path(self, si):
        r = self.rows.get(si)
        rel = (r.get("image") if r else None) or "Actual_Frames/frame_%05d.png" % si
        return os.path.join(self.dir, rel.replace("\\", "/"))

    def frame(self, si):
        if si in self._full:
            self._full.move_to_end(si)
            return self._full[si]
        p = self.frame_path(si)
        f = None
        if os.path.isfile(p):
            try:
                w, h, ct, plte, ncols, rows = decode_native(p, self.decoder)
                f = Frame(w, h, rows_to_rgb(rows, ct, plte, ncols), ncols)
                self.decoded += 1
                if self.size is None:
                    self.size = (w, h)
            except (PngError, OSError, zlib.error, ValueError):
                f = None
        self._full[si] = f
        while len(self._full) > 48:
            self._full.popitem(last=False)
        return f

    def frame_part(self, si, max_rows, max_cols):
        if si in self._full and self._full[si] is not None:
            return self._full[si]
        p = self.frame_path(si)
        if not os.path.isfile(p):
            return None
        try:
            w, h, ct, plte, ncols, rows = decode_native(p, self.decoder, max_rows, max_cols)
        except (PngError, OSError, zlib.error, ValueError):
            return None
        self.decoded_partial += 1
        if self.size is None:
            self.size = (w, h)
        return Frame(w, h, rows_to_rgb(rows, ct, plte, ncols), ncols)

    def mask(self, si):
        if si in self._mask:
            return self._mask[si]
        p = os.path.join(self.dir, "target_mask", "frame_%05d.png" % si)
        m = None
        if os.path.isfile(p):
            try:
                w, h, ct, plte, ncols, rows = decode_native(p, self.decoder)
                m = rows_to_gray(rows, ct, plte, ncols)
            except (PngError, OSError, zlib.error, ValueError):
                m = None
        self._mask[si] = m
        while len(self._mask) > 160:
            self._mask.popitem(last=False)
        return m

    def pose(self, si):
        v = (self.rows.get(si) or {}).get("view") or {}
        return v.get("origin"), v.get("rot")

    def brk(self, a):
        ra, rb = self.rows.get(a), self.rows.get(a + 1)
        if ra is None or rb is None:
            return "session_index missing"
        fa, fb = ra.get("frame_index"), rb.get("frame_index")
        if fa is None or fb is None or fb - fa != 1:
            return "engine-frame jump"
        for si in (a, a + 1):
            if not os.path.isfile(self.frame_path(si)):
                return "frame png missing"
        return None

    def brk52(self, a):
        ra, rb = self.rows.get(a), self.rows.get(a + 1)
        if ra is None or rb is None:
            return "session_index missing"
        fa, fb = ra.get("frame_index"), rb.get("frame_index")
        if fa is None or fb is None:
            return "no frame_index"
        if fb - fa != 1:
            return "engine-frame jump"
        return None


def angd(a, b):
    return abs(((a - b) + 180.0) % 360.0 - 180.0)


def moved(s, sis):
    base = None
    for si in sis:
        o, r = s.pose(si)
        if o is None or r is None:
            return True
        if base is None:
            base = (o, r)
            continue
        if max(abs(float(x) - float(y)) for x, y in zip(o, base[0])) > MOVE_CM:
            return True
        if max(angd(float(x), float(y)) for x, y in zip(r, base[1])) > MOVE_DEG:
            return True
    return False


def moved52(s, sis):
    if not sis:
        return False
    o0, r0 = s.pose(sis[0])
    o0 = [float(x) for x in (o0 or [0, 0, 0])]
    r0 = [float(x) for x in (r0 or [0, 0, 0])]
    for si in sis[1:]:
        o, r = s.pose(si)
        o = [float(x) for x in (o or [0, 0, 0])]
        r = [float(x) for x in (r or [0, 0, 0])]
        if max(abs(a - b) for a, b in zip(o, o0)) > MOVE_CM or max(angd(a, b) for a, b in zip(r, r0)) > MOVE_DEG:
            return True
    return False


def ent_type(a):
    return TYPE_MAP.get(a.get("id"), a.get("id"))


def infer_reasons(si, ev, typ):
    L = ev["Lset"]
    if si in L:
        return ("temporal_aa",)
    runs = ev["runs"]
    if runs and si > runs[0][1]:
        return ("hide_return",) if typ in HIDE else ("temporal_aa",)
    return ("unknown",)


def events_of(s):
    out = []
    for i, a in enumerate(s.anno.get("anomalies") or []):
        typ = TYPE_MAP.get(a.get("anomaly_type"), a.get("anomaly_type"))
        inj = ((a.get("injected_frames") or a.get("affected_frames") or {}).get("frame_indices")) or []
        nodes = ((a.get("affected_objects") or {}).get("nodes")) or []
        node = nodes[0].get("name") if nodes else None
        L = sorted(set(x for x in inj if isinstance(x, int)))
        starts = collections.Counter()
        for si in L:
            for x in (s.rows.get(si) or {}).get("anomalies") or []:
                if ent_type(x) == typ and (node is None or x.get("target_name") == node):
                    starts[x.get("start_frame")] += 1
        sf = starts.most_common(1)[0][0] if starts else None
        ev = dict(ord=i, type=typ, L=L, Lset=set(L), runs=runs_of(L), node=node, start_frame=sf, entries={},
                  T=set(), reasons={}, mv={}, bbox={}, disagree=0, has_reason=False)
        if sf is not None:
            for si in s.sis:
                for x in (s.rows[si].get("anomalies") or []):
                    if x.get("start_frame") == sf and ent_type(x) == typ and (node is None or x.get("target_name") == node):
                        ev["entries"][si] = x
                        if x.get("transition") == 1:
                            ev["T"].add(si)
                            rs = x.get("transition_reason")
                            if isinstance(rs, list) and rs:
                                ev["reasons"][si] = tuple(str(q) for q in rs)
                                ev["has_reason"] = True
                        ev["mv"][si] = x.get("mask_value")
                        ev["bbox"][si] = x.get("bbox_px")
                        if "labelled" in x and bool(x.get("labelled")) != (si in ev["Lset"]):
                            ev["disagree"] += 1
                        break
        for si in ev["T"]:
            if si not in ev["reasons"]:
                ev["reasons"][si] = infer_reasons(si, ev, typ)
        out.append(ev)
    return out


def session_rule(s):
    has_labelled = False
    has_flags = any(str(k).startswith("label_transition") for k in s.summary)
    for si in s.sis:
        for x in (s.rows[si].get("anomalies") or []):
            if "labelled" in x:
                has_labelled = True
                break
            if x.get("transition") == 1:
                has_flags = True
        if has_labelled:
            break
    taa = s.summary.get("label_temporal_aa")
    taa = None if taa is None else bool(taa)
    if has_labelled:
        rule = "new"
    elif has_flags:
        rule = "flags"
    else:
        rule = "old"
    return rule, taa


def _eq_ascii(v):
    t = _EQ_ASCII.get(v)
    if t is None:
        t = bytes(49 if i == v else 48 for i in range(256))
        _EQ_ASCII[v] = t
    return t


def row_bits_eq(row, v):
    if v not in row:
        return 0
    return int(row.translate(_eq_ascii(v))[::-1], 2)


def dilate_rows(rows, r, width):
    if r <= 0 or not any(rows):
        return list(rows)
    full = (1 << width) - 1
    widths = {}
    for dy in range(-r, r + 1):
        widths[dy] = math.isqrt(r * r - dy * dy)
    hd = {}
    for w in set(widths.values()):
        lst = []
        for v in rows:
            acc = v
            if v:
                for k in range(1, w + 1):
                    acc |= (v << k) | (v >> k)
                acc &= full
            lst.append(acc)
        hd[w] = lst
    n = len(rows)
    out = [0] * n
    for y in range(n):
        acc = 0
        for dy in range(-r, r + 1):
            yy = y + dy
            if 0 <= yy < n:
                acc |= hd[widths[dy]][yy]
        out[y] = acc
    return out


def roi_for(s, ev, L, mv, bboxes, W, H):
    if mv:
        acc = [0] * H
        n = 0
        for si in L:
            m = s.mask(si)
            if m is None:
                continue
            hit = False
            for y in range(min(H, len(m))):
                b = row_bits_eq(m[y], mv)
                if b:
                    acc[y] |= b
                    hit = True
            if hit:
                n += 1
        if n and sum(popcount(v) for v in acc) >= 64:
            return dilate_rows(acc, ROI_DILATE, W), "mask"
    b = [x for x in bboxes if x and len(x) == 4 and x[2] > 8 and x[3] > 8]
    if b:
        x, y, w, h = (median([q[i] for q in b]) for i in range(4))
        x0, y0, x1, y1 = int(max(0, x)), int(max(0, y)), int(min(W, x + w)), int(min(H, y + h))
        if x1 - x0 > 8 and y1 - y0 > 8:
            rowbits = ((1 << (x1 - x0)) - 1) << x0
            return [rowbits if y0 <= yy < y1 else 0 for yy in range(H)], "bbox"
    return None, None


def _interleave_stride(row, W):
    ws = (W + DS - 1) // DS
    o = bytearray(3 * ws)
    step = 3 * DS
    o[0::3] = row[0::step]
    o[1::3] = row[1::step]
    o[2::3] = row[2::step]
    return bytes(o)


def _maxch(a, b):
    d = bytes(map(abs, map(_sub, a, b)))
    return bytes(map(max, d[0::3], d[1::3], d[2::3]))


def _median_bytes(rows):
    k = len(rows)
    if k == 1:
        return rows[0]
    srt = list(map(sorted, zip(*rows)))
    if k % 2:
        return bytes(map(operator.itemgetter(k // 2), srt))
    lo = map(operator.itemgetter(k // 2 - 1), srt)
    hi = map(operator.itemgetter(k // 2), srt)
    return bytes(map(operator.rshift, map(_add, lo, hi), itertools.repeat(1)))


def _median2_list(rows):
    k = len(rows)
    srt = list(map(sorted, zip(*rows)))
    if k % 2:
        return [2 * v for v in map(operator.itemgetter(k // 2), srt)]
    return list(map(_add, map(operator.itemgetter(k // 2 - 1), srt), map(operator.itemgetter(k // 2), srt)))


def _largest_component(strong):
    seen = set()
    best = 0
    pts = set()
    for ys, bits in strong.items():
        for a, b in bit_runs(bits):
            for x in range(a, b):
                pts.add((ys, x))
    for p in pts:
        if p in seen:
            continue
        seen.add(p)
        stack = [p]
        n = 0
        while stack:
            y, x = stack.pop()
            n += 1
            for q in ((y - 1, x), (y + 1, x), (y, x - 1), (y, x + 1)):
                if q in pts and q not in seen:
                    seen.add(q)
                    stack.append(q)
        if n > best:
            best = n
    return best


class Ctx(object):
    def __init__(self, roi_rows, W, H, fullframe):
        self.W, self.H = W, H
        self.roi_rows = roi_rows
        ys = [y for y, v in enumerate(roi_rows) if v]
        self.y0 = max(0, ys[0] - 8)
        self.y1 = min(H, ys[-1] + 9)
        xmin = min((v & -v).bit_length() - 1 for v in roi_rows if v)
        xmax = max(v.bit_length() - 1 for v in roi_rows if v)
        self.x0 = max(0, xmin - 8)
        self.x1 = min(W, xmax + 9)
        self.n_roi = sum(popcount(v) for v in roi_rows)
        self.crop = []
        for y in ys:
            runs = bit_runs(roi_rows[y])
            s0, e0 = runs[0][0], runs[-1][1]
            self.crop.append((y, s0, e0, [(a - s0, b - s0) for a, b in runs]))
        self.fullframe = fullframe
        self.Hs = (H + DS - 1) // DS
        self.Ws = (W + DS - 1) // DS
        self.out_rows = None
        self.dyn_px = 0

    def view(self, f):
        if f is None:
            return None
        crop = {y: f.rows[y][3 * s0:3 * e0] for y, s0, e0, _r in self.crop}
        if self.fullframe:
            return (crop, None)
        st = [_interleave_stride(f.rows[ys * DS], f.w) for ys in range(self.Hs)]
        return (crop, st)

    def build_out(self, ref_views):
        if self.fullframe:
            return
        Hs, Ws = self.Hs, self.Ws
        roi_s = []
        for ys in range(Hs):
            R = 0
            for y in range(ys * DS, min(self.H, ys * DS + DS)):
                R |= self.roi_rows[y]
            v = 0
            if R:
                s = bin(R)[2:][::-1]
                for xs in range(Ws):
                    if "1" in s[xs * DS:xs * DS + DS]:
                        v |= 1 << xs
            roi_s.append(v)
        dyn = [0] * Hs
        for ys in range(Hs):
            cols = [rv[1][ys] for rv in ref_views]
            med2 = _median2_list(cols)
            for c in cols:
                d2 = list(map(abs, map(_sub, map(operator.lshift, c, itertools.repeat(1)), med2)))
                m = bytes(map((2 * 24).__lt__, map(max, d2[0::3], d2[1::3], d2[2::3])))
                if 1 in m:
                    dyn[ys] |= int(m.translate(_ONE_ASCII)[::-1], 2)
        dyn = dilate_rows(dyn, 2, Ws)
        roi_d = dilate_rows(roi_s, OUT_DILATE // DS, Ws)
        full = (1 << Ws) - 1
        top = int(0.14 * Hs)
        out = []
        for ys in range(Hs):
            v = full & ~roi_d[ys] & ~dyn[ys]
            if ys < top:
                v = 0
            if v:
                out.append((ys, v))
        self.out_rows = out
        self.dyn_px = sum(popcount(v) for v in dyn) * DS * DS


def metrics(v, rv, ctx, want_ob=True):
    dsum = 0
    pcount = 0
    vc, rc = v[0], rv[0]
    for y, _s0, _e0, runs in ctx.crop:
        m = _maxch(vc[y], rc[y])
        for a, b in runs:
            seg = m[a:b]
            dsum += sum(seg)
            pcount += len(seg.translate(None, _LE8))
    D = dsum / float(ctx.n_roi)
    P = pcount / float(ctx.n_roi)
    OB = 0
    if want_ob and ctx.out_rows and v[1] is not None and rv[1] is not None:
        strong = {}
        for ys, orow in ctx.out_rows:
            m = _maxch(v[1][ys], rv[1][ys])
            if not m.translate(None, _LE48):
                continue
            bits = int(m.translate(_GT48_ASCII)[::-1], 2) & orow
            if bits:
                strong[ys] = bits
        if strong:
            OB = _largest_component(strong) * DS * DS
    return D, P, OB


def median_view(views, with_st=True):
    crop = {}
    keys = views[0][0].keys()
    for y in keys:
        crop[y] = _median_bytes([v[0][y] for v in views])
    st = None
    if with_st and views[0][1] is not None:
        st = [_median_bytes([v[1][ys] for v in views]) for ys in range(len(views[0][1]))]
    return (crop, st)


def noise_model(views, ctx, want_ob=True):
    nD, nP, nOB = [], [], []
    k = len(views)
    for i in range(k):
        Ri = median_view(views[:i] + views[i + 1:], want_ob) if k >= 3 else views[1 - i]
        D, P, OB = metrics(views[i], Ri, ctx, want_ob)
        nD.append(D)
        nP.append(P)
        nOB.append(OB)
    muD = median(nD)
    sdD = max(robust_sigma(nD), pstdev(nD), 0.15)
    return muD, max(muD + K_SIG * sdD, muD + D_FLOOR), max(max(nP) * 2.0, P_FLOOR), max(max(nOB) * 2, OUT_BLOB_MIN)


def transition_gate(L, runs, V, span, reasons, drop, sigma, taa, partial_class=None):
    Ls = set(L)
    sp = set(span)
    V = set(V) & sp
    unl = sorted(V - Ls)
    lnv = sorted((Ls & sp) - V)
    exc_unl, exc_lnv = set(), set()
    stats = collections.Counter()
    run_of = {}
    for a, b in runs:
        for si in range(a, b + 1):
            run_of[si] = (a, b)
    unbacked = []
    for si in lnv:
        rs = reasons.get(si) or ()
        a, b = run_of.get(si, (si, si))
        onset_taa = "temporal_aa" in rs and si <= (a + b) / 2.0
        if "partial" in rs and partial_class is not None:
            c = partial_class(si)
            if c:
                exc_lnv.add(si)
                stats["partial_" + c] += 1
                continue
            if not onset_taa:
                stats["partial_unbacked"] += 1
                unbacked.append(si)
                continue
        if onset_taa:
            exc_lnv.add(si)
            stats["onset_excused"] += 1
            continue
        if "unresolved" in rs:
            stats["unresolved_off"] += 1
    ends = sorted(b for _a, b in runs)
    starts = sorted(a for a, _b in runs)
    for si in unl:
        rs = set(reasons.get(si) or ())
        if not (rs & set(AA_ONLY_REASONS)):
            continue
        prior = [b for b in ends if b < si]
        if not prior:
            continue
        b = prior[-1]
        nxt = [a for a in starts if a > b]
        if nxt and si >= nxt[0]:
            continue
        if all((x in V) for x in range(b + 1, si + 1)):
            exc_unl.add(si)
            stats["tail_excused"] += 1
    decay = []
    for a, b in runs:
        after = [si for si in exc_unl if si > b and not any(a2 > b and a2 <= si for a2 in starts)]
        if not after or drop is None or sigma is None:
            continue
        if (b + 1) not in drop or b not in drop:
            continue
        ok = drop[b + 1] < drop[b] - DECAY_K * sigma
        decay.append((b, ok))
    unl_x = [si for si in unl if si not in exc_unl]
    lnv_x = [si for si in lnv if si not in exc_lnv]
    fails = []
    if unl_x:
        fails.append("unlabelled visible")
    if lnv_x:
        fails.append("labelled not visible")
    if any(not ok for _b, ok in decay):
        fails.append("excused tail did not decay")
    aa_flags = [si for si, rs in reasons.items() if set(rs) & set(AA_ONLY_REASONS)]
    if taa is False and aa_flags:
        fails.append("anti-aliasing flag without temporal AA")
    return dict(unl=unl, lnv=lnv, unl_x=unl_x, lnv_x=lnv_x, exc_unl=exc_unl, exc_lnv=exc_lnv, stats=stats,
                decay=decay, fails=fails, unbacked=unbacked, strict00=not unl and not lnv)


def split_censored(tg, L, on_c, off_c):
    L = sorted(L)
    L0, L1 = L[0], L[-1]
    mid = (L0 + L1) / 2.0
    fails = []
    amb = []
    unl = [si for si in tg["unl_x"] if not ((si > L1 and off_c) or (si < L0 and on_c))]
    amb += [si for si in tg["unl_x"] if (si > L1 and off_c) or (si < L0 and on_c)]
    lnv = [si for si in tg["lnv_x"] if not ((si > mid and off_c) or (si <= mid and on_c))]
    amb += [si for si in tg["lnv_x"] if (si > mid and off_c) or (si <= mid and on_c)]
    if unl:
        fails.append("unlabelled visible")
    if lnv:
        fails.append("labelled not visible")
    bad_decay = any(not ok for _b, ok in tg["decay"])
    if bad_decay and not off_c:
        fails.append("excused tail did not decay")
    if bad_decay and off_c:
        amb.append(-1)
    fails += [x for x in tg["fails"] if x == "anti-aliasing flag without temporal AA"]
    return fails, amb


def ta_offsets(vis_window, run_frames, exc_unl, exc_lnv):
    vv = sorted(si for si in vis_window if si not in exc_unl)
    ll = sorted(si for si in run_frames if si not in exc_lnv)
    if not vv or not ll:
        return None, None
    return vv[0] - ll[0], vv[-1] - ll[-1]


def ce_witness(s, typ, sf):
    key = "%s@%s" % (typ if typ != "blinking" else "blinking", sf)
    rows = [r for r in s.ce_rows if r.get("event") == key]
    if not rows and typ == "blinking":
        rows = [r for r in s.ce_rows if r.get("event") == "blink@%s" % sf]
    if not rows:
        return None
    ph = collections.defaultdict(dict)
    for r in rows:
        n = r.get("chg_n") or -1
        if not r.get("chg_measured") or n <= 0:
            ph[r.get("phase_ordinal", 0)][r.get("window_index", 0)] = None
            continue
        ref = r.get("ref_gt8")
        ph[r.get("phase_ordinal", 0)][r.get("window_index", 0)] = dict(
            ref=(ref / float(n)) if (ref is not None and ref >= 0) else None)
    out = []
    for p in sorted(ph):
        w = ph[p]
        w0 = w.get(0)
        later = [w[k]["ref"] for k in (1, 2, 3) if w.get(k) and w[k]["ref"] is not None]
        if not w0 or w0.get("ref") is None or not later:
            out.append("unmeasured")
            continue
        mx = max(later)
        if mx < 0.05:
            out.append("no-change")
        elif w0["ref"] >= 0.5 * mx:
            out.append("onset-on-first")
        elif w0["ref"] < 0.1 * mx:
            out.append("label-early")
        else:
            out.append("partial-first")
    return out


def analyse_d(s, ev, evs, taa):
    res = dict(type=ev["type"], path="d", status=None)
    L = list(ev["L"])
    if not L:
        res["status"] = "NO-LABELLED-FRAMES"
        return res
    Ls = set(L)
    Ts = set(ev["T"])
    mine = set(ev["entries"]) | Ls | Ts
    other = set()
    for o in evs:
        if o is ev:
            continue
        other.update(o["L"])
        other.update(o["T"])
        other.update(o["entries"])
    other -= mine
    lo, hi = L[0], L[-1]
    tail = max([x - hi for x in Ts if x > hi] or [0])
    span_lo, span_hi = lo - SPAN_PAD, hi + SPAN_PAD + tail
    for si in range(lo - 1, lo - SPAN_PAD - 1, -1):
        if si in other:
            span_lo = si + 1
            break
    for si in range(hi + 1, span_hi + 1):
        if si in other:
            span_hi = si - 1
            break
    span_lo = max(span_lo, s.sis[0])
    span_hi = min(span_hi, s.sis[-1])
    span = [si for si in range(span_lo, span_hi + 1) if si in s.rows]

    def clean(si):
        if si not in s.rows or si in Ls or si in Ts or si in other:
            return False
        return not (s.rows[si].get("anomaly_present") and si not in mine)

    def pre_clean(si):
        return clean(si) and si not in ev["entries"]

    prev = [x for x in other if x < lo]
    nxt_o = [x for x in other if x > hi]
    prev_last = max(prev) if prev else None
    start = lo - REF_BACK if prev_last is None else max(lo - REF_BACK, prev_last + PREV_SETTLE + 1)
    refc = [si for si in range(start, lo) if pre_clean(si)]
    if len(refc) < REF_MIN:
        tailx = max([hi] + [x for x in Ts if x > hi])
        lim = min(nxt_o) if nxt_o else tailx + 1 + POST_SKIP + POST_REF_N
        post = [si for si in range(tailx + 1 + POST_SKIP, lim) if clean(si)][:POST_REF_N]
        if len(refc) + len(post) < REF_MIN:
            res["status"] = "CONFOUNDED-REFERENCE"
            return res
        refc = sorted(refc + post)
    if lo < SETTLE_SKIP or max(refc) < SETTLE_SKIP:
        res["status"] = "WARMUP"
        return res
    if moved(s, refc + span):
        res["status"] = "CAMERA-MOVED"
        return res
    mvs = collections.Counter(v for si, v in ev["mv"].items() if si in Ls and v)
    mv = mvs.most_common(1)[0][0] if mvs else None
    bb = [ev["bbox"][si] for si in L if ev["bbox"].get(si)]
    f0 = s.frame(refc[0])
    if f0 is None:
        res["status"] = "NO-FRAMES"
        return res
    roi, src = roi_for(s, ev, L, mv, bb, f0.w, f0.h)
    if roi is None:
        res["status"] = "NO-ROI"
        return res
    ctx = Ctx(roi, f0.w, f0.h, False)
    res["roi_src"] = src
    vc = {}

    def V(si):
        if si not in vc:
            vc[si] = ctx.view(s.frame(si))
        return vc[si]

    rv = [V(si) for si in refc]
    if any(x is None for x in rv):
        res["status"] = "NO-FRAMES"
        return res
    ctx.build_out(rv)
    lab_runs = runs_of(Ls)
    series = {}
    obflag = {}
    cens = {}
    sig_by_si = {}
    run_info = []
    fallback_runs = 0
    for i, (a, b) in enumerate(lab_runs):
        lo_w = (lab_runs[i - 1][1] + a) // 2 + 1 if i > 0 else span_lo
        hi_w = (b + lab_runs[i + 1][0]) // 2 if i + 1 < len(lab_runs) else span_hi
        if i == 0:
            ron = refc
        else:
            ron = [si for si in range(lab_runs[i - 1][1] + 1, a) if clean(si)]
            if len(ron) < 2:
                ron = refc
        nxt = lab_runs[i + 1][0] if i + 1 < len(lab_runs) else span_hi + 1
        roff = [si for si in range(b + 1, nxt) if clean(si)][:POST_REF_N]
        on_views = [V(x) for x in ron]
        if any(x is None for x in on_views):
            continue
        Ron = median_view(on_views)
        mu_on, thD_on, thP_on, thOB_on = noise_model(on_views, ctx)
        off_views = [V(x) for x in roff] if len(roff) >= 2 else []
        off_ok = len(off_views) >= 2 and not any(x is None for x in off_views)
        midr = (a + b) / 2.0
        ser_on = {}
        ser_off = {}
        for si in range(lo_w, hi_w + 1):
            if si not in s.rows:
                continue
            v = V(si)
            if v is None:
                continue
            D, P, OB = metrics(v, Ron, ctx)
            ser_on[si] = (D, P)
            obflag[si] = OB > thOB_on
        settled = False
        k = 0
        if off_ok:
            while len(off_views) - k >= 3:
                rest = off_views[k + 1:]
                _m1, thD1, thP1, _t1 = noise_model(rest, ctx, False)
                d_first, p_first, _o = metrics(off_views[k], median_view(rest, False), ctx, False)
                if d_first <= thD1 and p_first <= thP1:
                    settled = True
                    break
                k += 1
            if not settled and len(off_views) - k == 2:
                d_first, p_first, _o = metrics(off_views[k], off_views[k + 1], ctx, False)
                settled = d_first <= thD_on and p_first <= thP_on
        if settled:
            use = off_views[k:]
            Roff = median_view(use, False)
            mu_off, thD_off, thP_off, _t = noise_model(use, ctx, False)
            for si in ser_on:
                if si > midr:
                    D, P, _o = metrics(V(si), Roff, ctx, False)
                    ser_off[si] = (D, P)
            late = [si for si in range(roff[-1] + 1, hi_w + 1) if si in ser_on and clean(si)]
            dD = max([v[0] for k, v in ser_off.items()] or [0.0])
            dP = max([v[1] for k, v in ser_off.items()] or [0.0])
            if len(late) >= 2:
                Rl = median_view([V(x) for x in late], False)
                lD, lP, _o = metrics(Rl, Roff, ctx, False)
                drift = max((lD - mu_off) / max(dD - mu_off, 1e-6), lP / max(dP, 1e-6))
                if drift >= REL:
                    cens[b] = "scene drift"
        else:
            fallback_runs += 1
            mu_off, thD_off, thP_off = mu_on, thD_on, thP_on
            for si in ser_on:
                if si > midr:
                    ser_off[si] = ser_on[si]
            late = [si for si in range(b + 4, hi_w + 1) if si in ser_on and si not in Ls and si not in Ts]
            dD = max(v[0] for v in ser_on.values()) if ser_on else 0.0
            dP = max(v[1] for v in ser_on.values()) if ser_on else 0.0
            if b + SUFFIX > span_hi:
                cens[b] = "no suffix"
            elif len(late) >= 2 and not any(V(x) is None for x in late):
                Rl = median_view([V(x) for x in late], False)
                lD, lP, _o = metrics(Rl, Ron, ctx, False)
                drift = max((lD - mu_on) / max(dD - mu_on, 1e-6), lP / max(dP, 1e-6))
                if drift >= REL:
                    cens[b] = "scene drift"
        on_items = [(si, ser_on[si]) for si in ser_on if si <= midr]
        off_items = [(si, ser_off[si]) for si in ser_off if si > midr]
        dD_on = max([x[1][0] for x in on_items] or [0.0])
        dD_off = max([x[1][0] for x in off_items] or [dD_on])
        for si, (D, P) in on_items:
            series[si] = dict(D=D, P=P, frac=(D - mu_on) / max(dD_on - mu_on, 1e-6), strict=bool(D > thD_on or P > thP_on))
            sig_by_si[si] = (thD_on - mu_on) / K_SIG
        for si, (D, P) in off_items:
            series[si] = dict(D=D, P=P, frac=(D - mu_off) / max(dD_off - mu_off, 1e-6), strict=bool(D > thD_off or P > thP_off))
            sig_by_si[si] = (thD_off - mu_off) / K_SIG
        run_info.append(dict(a=a, b=b, lo=lo_w, hi=hi_w, thrD_on=thD_on, depth_on=dD_on, fallback=not settled))
    if not series:
        res["status"] = "NO-FRAMES"
        return res
    peak = max(series, key=lambda si: series[si]["D"])
    res["confounded"] = not (min(L) - 1 <= peak <= max(L) + SUFFIX + tail)
    depthD = max(r["depth_on"] for r in run_info)
    depthP = max(series[si]["P"] for si in series)
    res["measurable"] = bool(depthP >= MEAS_P or any(r["depth_on"] >= MEAS_K * r["thrD_on"] for r in run_info))
    gap = {}
    for a, b in lab_runs:
        w = s.brk(a - 1)
        if w and w != "engine-frame jump":
            cens.setdefault(("s", a), w)
        if w == "engine-frame jump":
            gap[("s", a)] = w
        w = s.brk(b)
        if w and w != "engine-frame jump":
            cens.setdefault(("e", b), w)
        if w == "engine-frame jump":
            gap[("e", b)] = w
    drop = {si: series[si]["frac"] for si in series}
    sig = median(list(sig_by_si.values())) / max(depthD, 1e-6) if sig_by_si else None
    reasons = {si: ev["reasons"].get(si, ()) for si in Ts}
    on_c = ("s", lab_runs[0][0]) in cens
    off_c = (lab_runs[-1][1] in cens) or (("e", lab_runs[-1][1]) in cens)
    per = {}
    for name, t in THRESH:
        vis = set(si for si, v in series.items() if v["strict"] and v["frac"] >= t)
        tg = transition_gate(L, lab_runs, vis, [si for si in span if si in series], reasons, drop, sig, taa)
        fails, amb = split_censored(tg, L, on_c, off_c)
        raw_fails = []
        if [si for si in tg["unl"] if not ((si > max(L) and off_c) or (si < min(L) and on_c))]:
            raw_fails.append("unlabelled visible")
        mid_all = (min(L) + max(L)) / 2.0
        if [si for si in tg["lnv"] if not ((si > mid_all and off_c) or (si <= mid_all and on_c))]:
            raw_fails.append("labelled not visible")
        edges = []
        for ri in run_info:
            a, b = ri["a"], ri["b"]
            win = [si for si in vis if ri["lo"] <= si <= ri["hi"]]
            vv = sorted(win)
            e = dict(a=a, b=b, start=(vv[0] - a) if vv else None, end=(vv[-1] - b) if vv else None,
                     cs=("s", a) in cens, ce=(b in cens) or (("e", b) in cens), gs=("s", a) in gap, ge=("e", b) in gap,
                     fb=ri["fallback"])
            e["cr"] = cens.get(b) or cens.get(("e", b)) or cens.get(("s", a))
            e["us"] = bool(vv) and e["start"] < 0 and ri["lo"] < a and ri["lo"] in vis
            e["ue"] = bool(vv) and e["end"] > 0 and ri["hi"] > b and ri["hi"] in vis
            st, en = ta_offsets(win, range(a, b + 1), tg["exc_unl"], tg["exc_lnv"])
            e["start_ta"], e["end_ta"] = st, en
            edges.append(e)
        cen_any = any(e["cs"] or e["ce"] for e in edges)
        if not res["measurable"]:
            v_ta = v_raw = "NOT-MEASURABLE"
        elif res["confounded"]:
            v_ta = v_raw = "CONFOUNDED"
        else:
            v_ta = "FAIL" if fails else ("CENSORED" if cen_any else "PASS")
            v_raw = "FAIL" if raw_fails else ("CENSORED" if cen_any else "PASS")
        per[name] = dict(verdict=v_ta, verdict_raw=v_raw, edges=edges, fails=fails, stats=dict(tg["stats"]),
                         excused=len(tg["exc_unl"]) + len(tg["exc_lnv"]), strict00=tg["strict00"])
    res["per"] = per
    res["status"] = "OK"
    res["fallback_runs"] = fallback_runs
    res["runs"] = len(run_info)
    res["post1"] = []
    for ri in run_info:
        x = series.get(ri["b"] + 1)
        if x is not None and (ri["b"] + 1) not in Ls:
            res["post1"].append(round(max(0.0, x["frac"]), 4))
    res["wrong_obj"] = any(obflag.get(si) for si in L)
    res["wrong_obj_clean"] = any(v for si, v in obflag.items() if si not in Ls)
    mm, mx = [], []
    if mv:
        for si in L:
            m = s.mask(si)
            if m is None or not any(mv in row for row in m):
                mm.append(si)
        for si in span:
            if si in Ls:
                continue
            m = s.mask(si)
            if m is not None and any(mv in row for row in m):
                mx.append(si)
    else:
        mm = list(L)
    res["mask_missing"] = len(mm)
    res["mask_extra"] = len(mx)
    return res


def events52(s):
    ev = collections.OrderedDict()
    for si in s.sis:
        for a in (s.rows[si].get("anomalies") or []):
            if a.get("id") != M52:
                continue
            e = ev.setdefault(a.get("start_frame"), dict(start_frame=a.get("start_frame"), target=a.get("target_name"),
                                                         rows=[], held={}, rstate={}, bbox={}, entries={}))
            e["rows"].append(si)
            e["held"][si] = bool(a.get("stuck_mip.held"))
            e["rstate"][si] = a.get("stuck_mip.render_state")
            e["bbox"][si] = a.get("bbox_px")
            e["entries"][si] = a
    return list(ev.values())


def roi_of52(bboxes, W, H):
    b = [x for x in bboxes if x and len(x) == 4 and x[2] > 8 and x[3] > 8]
    if not b:
        return None
    x, y, w, h = (median([q[i] for q in b]) for i in range(4))
    x0 = int(x + w * ROI_SHRINK)
    y0 = int(y + h * ROI_SHRINK)
    x1 = int(x + w * (1 - ROI_SHRINK))
    y1 = int(y + h * (1 - ROI_SHRINK))
    x0 = max(0, x0)
    y0 = max(0, y0)
    x1 = min(W, x1)
    y1 = min(H, y1)
    return (x0, y0, x1, y1) if x1 - x0 > 8 and y1 - y0 > 8 else None


def _grey_row(row, x0, x1):
    seg = row[3 * x0:3 * x1]
    return list(map(_add, map(_add, map(_T299.__getitem__, seg[0::3]), map(_T587.__getitem__, seg[1::3])),
                    map(_T114.__getitem__, seg[2::3])))


def sharpness(f, roi):
    x0, y0, x1, y1 = roi
    g = [_grey_row(f.rows[y], x0, x1) for y in range(y0, y1)]
    tot = 0
    for j in range(len(g) - 1):
        a = g[j]
        nb = g[j + 1]
        tot += sum(map(abs, map(_sub, a[1:], a[:-1])))
        tot += sum(map(abs, map(_sub, nb[:-1], a[:-1])))
    n = (y1 - y0 - 1) * (x1 - x0 - 1)
    return tot / (1000.0 * n) if n > 0 else 0.0


def tables52(s, rois):
    rois = [r for r in rois if r is not None]
    out = {r: {} for r in rois}
    if not rois:
        return out
    my = max(r[3] for r in rois)
    mx = max(r[2] for r in rois)
    for si in s.sis:
        f = s.frame_part(si, my, mx)
        if f is None:
            continue
        for r in rois:
            out[r][si] = sharpness(f, r)
    return out


def local_sigma(rho, lo, hi, excl, win=NOISE_WIN):
    si = [x for x in range(lo - win, hi + win + 1) if x in rho and x not in excl]
    if len(si) < 12:
        si = [x for x in range(lo - 3 * win, hi + 3 * win + 1) if x in rho and x not in excl]
    if len(si) < 12:
        return None, len(si)
    v = [rho[x] for x in si]
    rm = rolling_median(v)
    md = median(v)
    res = [(a - b) / md for a, b in zip(v, rm)] if md else [0.0] * len(v)
    return robust_sigma(res), len(si)


def _linfit_resid(xs, ys):
    n = len(xs)
    mx = sum(xs) / n
    my = sum(ys) / n
    sxx = sum((x - mx) * (x - mx) for x in xs)
    k = sum((x - mx) * (y - my) for x, y in zip(xs, ys)) / sxx if sxx else 0.0
    c = my - k * mx
    return [y - (k * x + c) for x, y in zip(xs, ys)]


def window52(s, vis, span):
    hi = span[-1]
    fv = None
    onset_c = None
    for si in span:
        if si not in vis:
            continue
        if si >= hi:
            fv = si
            onset_c = "confirmation beyond span"
            break
        b = s.brk52(si)
        if b:
            fv = si
            onset_c = "confirmation crosses a break"
            break
        if si + 1 in vis:
            fv = si
            break
    if fv is not None and onset_c is None:
        if s.brk52(fv - 1):
            onset_c = "break beside the onset"
    lv = None
    offset_c = None
    conf = None
    S = set(span)
    if fv is not None:
        cand = fv
        clean = 0
        sb = None
        for si in range(fv + 1, hi + 1):
            b = s.brk52(si - 1)
            if b:
                clean = 0
                if sb is None:
                    sb = b
            if si not in S:
                continue
            if si in vis:
                cand = si
                clean = 0
                sb = None
            else:
                clean += 1
                if clean >= SUFFIX:
                    conf = si
                    break
        lv = cand
        if conf is None:
            offset_c = "no clean suffix"
        elif sb:
            offset_c = "break beside the offset"
    return fv, lv, conf, onset_c, offset_c


def record_any_held(entry):
    rts = entry.get("stuck_mip.render_textures") or []
    if not rts:
        return None
    forced = {}
    for t in entry.get("stuck_mip.textures") or []:
        if t.get("name") is not None and t.get("forced_mips") is not None:
            forced[t["name"]] = t["forced_mips"]
    known = False
    for t in rts:
        lv = t.get("level")
        if lv is not None:
            known = True
            if lv == "held":
                return True
            continue
        res = t.get("render_resident_mips")
        held = t.get("held_level_mips", forced.get(t.get("name")))
        if res is None or held is None or res < 0:
            continue
        known = True
        if res <= held:
            return True
    return False if known else None


def measure52(s, e, next_first, rho, roi, excl, has_record, label, T, reasons, taa):
    Lw = sorted(label)
    Tset = set(T)
    if not Lw:
        return dict(status="NO-LABEL")
    L0, L1 = Lw[0], Lw[-1]
    hi_cap = s.sis[-1]
    span_hi = min(L1 + SPAN_AFTER, hi_cap, (next_first - PRE_W - 1) if next_first is not None else hi_cap)
    span = [si for si in range(L0 - PRE_W, span_hi + 1) if si in s.rows and si in rho]
    if len(span) < PRE_W + len(Lw) + SUFFIX + 2:
        return dict(status="SHORT-SPAN")
    prew = [si for si in range(L0 - PRE_N, L0) if si in rho]
    if len(prew) < 4:
        return dict(status="NO-PRE-LEVEL")
    pre = median([rho[x] for x in prew])
    postw = span[-POST_N:] if span_hi - L1 >= 20 else []
    post = median([rho[x] for x in postw]) if len(postw) >= 4 else None
    own = set(range(L0 - 1, L1 + SPAN_AFTER // 2))
    ex = excl | own
    s_on, _n_on = local_sigma(rho, L0 - PRE_W, L0 - 1, ex)
    s_off, _n_off = local_sigma(rho, span_hi - 10, span_hi + 10, ex | set(range(L0 - 1, L1 + 12)))
    if s_on is None and s_off is None:
        return dict(status="NO-NOISE-MODEL")
    s_on = s_on if s_on is not None else s_off
    s_off = s_off if s_off is not None else s_on
    trend = 0.0
    tw = [x for x in range(L0 - 30, L0) if x in rho and x not in ex]
    if len(tw) >= 10:
        ys = [rho[x] for x in tw]
        md = median(ys)
        if md:
            trend = max(abs(r) / md for r in _linfit_resid([float(x) for x in tw], ys))
    thr_on = max(K_SIGMA * s_on, REL_FLOOR, trend)
    thr_off = max(K_SIGMA * s_off, REL_FLOOR, trend)
    cpre = sum(prew) / float(len(prew))
    cpost = sum(postw) / float(len(postw)) if postw else None
    ref = {}
    for si in span:
        if post is not None and cpost is not None and cpost > cpre:
            fr = min(1.0, max(0.0, (si - cpre) / (cpost - cpre)))
            ref[si] = pre + (post - pre) * fr
        else:
            ref[si] = pre
    drop = {si: 1.0 - rho[si] / ref[si] if ref[si] else 0.0 for si in span}
    inner = [drop[si] for si in span if L0 <= si <= L1 + SPAN_AFTER // 2]
    depth = max(inner) if inner else max(drop.values())
    mid = (L0 + L1) / 2.0
    thr = {si: (thr_on if si <= mid else thr_off) for si in span}
    strict = set(si for si in span if drop[si] > thr[si])
    measurable = depth >= DEPTH_K * max(thr_on, thr_off)
    drift_c = None
    if post is not None and pre:
        dr = post / pre - 1.0
        if abs(dr) > max(thr_off, 0.25 * max(depth, 1e-9)):
            drift_c = "picture does not return"
    rh = sorted(si for si, v in e["rstate"].items() if v == "held")
    ents = e["entries"]

    def pclass(si):
        if drop.get(si, 0.0) > thr.get(si, 0.0):
            return "confirmed"
        rec = record_any_held(ents.get(si) or {})
        return "invisible" if rec else None

    per = {}
    for name, frac in THRESH:
        vis = set(si for si in strict if drop[si] >= frac * depth)
        fv, lv, conf, onc, offc = window52(s, vis, span)
        if drift_c:
            offc = offc or drift_c
        tg = transition_gate(Lw, [[L0, L1]], vis, span, reasons, drop, s_off, taa, pclass)
        fails, amb = split_censored(tg, Lw, onc, offc)
        early = []
        if has_record:
            early = sorted(si for si in Lw if (not rh) or si < rh[0])
            if early:
                fails.append("labelled before the first render-held frame")
        Ls = set(Lw)
        inwin = set(si for si in vis if fv is not None and lv is not None and fv <= si <= lv)
        lb = [si for si in Lw if fv is None or si < fv]
        la = [si for si in Lw if lv is not None and si > lv]
        lb_def = [] if onc else lb
        la_def = [si for si in la if conf is not None and si > conf] if offc else la
        raw_fails = []
        if [si for si in inwin if si not in Ls]:
            raw_fails.append("unlabelled visible inside the pixel window")
        if [si for si in vis if si not in inwin and si not in Ls]:
            raw_fails.append("visible outside the pixel window")
        if lb_def:
            raw_fails.append("labelled before the first visible frame")
        if la_def:
            raw_fails.append("labelled after the last visible frame")
        cens = bool(onc or offc)
        v_ta = "FAIL" if fails else ("CENSORED" if cens else "PASS")
        v_raw = "FAIL" if raw_fails else ("CENSORED" if cens else "PASS")
        st = (fv - L0) if fv is not None else None
        en = (lv - L1) if lv is not None else None
        win = [si for si in inwin]
        sta, ena = ta_offsets(win, Lw, tg["exc_unl"], tg["exc_lnv"]) if win else (None, None)
        edges = [dict(a=L0, b=L1, start=st, end=en, start_ta=sta, end_ta=ena, cs=bool(onc), ce=bool(offc),
                      gs=False, ge=False, us=False, ue=False, fb=False)]
        per[name] = dict(verdict=v_ta, verdict_raw=v_raw, edges=edges, fails=fails, stats=dict(tg["stats"]),
                         excused=len(tg["exc_unl"]) + len(tg["exc_lnv"]), strict00=tg["strict00"])
    part_on = sum(1 for si in Lw if "partial" in (reasons.get(si) or ()) and si <= mid)
    part_off = sum(1 for si in Lw if "partial" in (reasons.get(si) or ()) and si > mid)
    post1 = []
    if (L1 + 1) in drop and depth > 0:
        post1.append(round(max(0.0, drop[L1 + 1] / depth), 4))
    return dict(status="OK", measurable=bool(measurable), per=per, depth=depth, part_on=part_on, part_off=part_off,
                post1=post1, warm=L0 < SETTLE_SKIP)


def analyse_session(d, decoder, types=None):
    s = Session(d, decoder)
    rule, taa = session_rule(s)
    evs = events_of(s)
    out = []
    by_start = {}
    for ev in evs:
        if ev["type"] == M52 and ev["start_frame"] is not None:
            by_start[ev["start_frame"]] = ev
    m52_res = {}
    if any(ev["type"] == M52 for ev in evs) and (types is None or M52 in types):
        m52_res = analyse_m52(s, evs, taa)
    for ev in evs:
        typ = ev["type"]
        if types and typ not in types:
            continue
        row = dict(type=typ, ord=ev["ord"], rule=rule, disagree=ev["disagree"], ce=None)
        if typ in NOT_JUDGEABLE:
            row["status"] = "NOT-JUDGEABLE"
            out.append(row)
            continue
        try:
            dres = analyse_d(s, ev, evs, taa)
        except Exception as ex:
            dres = dict(status="ERROR", error=repr(ex))
        if dres.get("status") == "OK" and ev["start_frame"] is not None:
            row["ce"] = ce_witness(s, typ, ev["start_frame"])
        if typ == M52:
            m = m52_res.get(ev["start_frame"])
            row["d"] = dres
            if m is not None and m.get("status") == "OK":
                row.update(path="m52", m52=m)
                if m.get("camera_moved"):
                    row["status"] = "CAMERA-MOVED"
                elif not m.get("measurable"):
                    row["status"] = "NOT-MEASURABLE"
                else:
                    row["status"] = "OK"
                    row["per"] = m["per"]
            else:
                row["path"] = "m52-fallback-d"
                row["m52_status"] = (m or {}).get("status")
                row.update(_d_status(dres))
        else:
            row["path"] = "d"
            row["d"] = dres
            row.update(_d_status(dres))
        out.append(row)
    return s, rule, taa, out


def _d_status(dres):
    st = dres.get("status")
    if st != "OK":
        return dict(status=st)
    v = dres["per"][RELEASE]["verdict"]
    if v in ("NOT-MEASURABLE", "CONFOUNDED"):
        return dict(status=v)
    return dict(status="OK", per=dres["per"])


def analyse_m52(s, evs, taa):
    ev52 = events52(s)
    if not ev52:
        return {}
    ann = {}
    reasons_by = {}
    for ev in evs:
        if ev["type"] == M52 and ev["start_frame"] is not None:
            ann[ev["start_frame"]] = ev["L"]
            reasons_by[ev["start_frame"]] = ev["reasons"]
    has_record = any(v for e in ev52 for v in e["rstate"].values())
    f0 = None
    for si in s.sis:
        f0 = s.frame_part(si, 1, 1)
        if f0 is not None:
            break
    if f0 is None:
        return {e["start_frame"]: dict(status="NO-FRAMES") for e in ev52}
    W, H = f0.w, f0.h

    def base_lab(e):
        return ann.get(e["start_frame"]) or sorted(si for si, v in e["held"].items() if v)

    targets = sorted(set(str(e["target"]) for e in ev52))
    rois = {}
    for t in targets:
        bbs = [b for e in ev52 if str(e["target"]) == t for b in e["bbox"].values()]
        rois[t] = roi_of52(bbs, W, H)
    by_roi = tables52(s, list(set(rois.values())))
    tables = {t: by_roi[r] for t, r in rois.items() if r is not None}
    firsts = [(base_lab(e) or e["rows"])[0] for e in ev52]

    def infl(lw):
        return set(range(lw[0] - 1, lw[-1] + SPAN_AFTER // 2))

    excl = set()
    for e in ev52:
        excl |= infl(base_lab(e) or e["rows"])
    out = {}
    for i, e in enumerate(ev52):
        t = str(e["target"])
        if rois.get(t) is None:
            out[e["start_frame"]] = dict(status="NO-ROI")
            continue
        lw = base_lab(e)
        if not lw:
            out[e["start_frame"]] = dict(status="NO-LABEL")
            continue
        own = base_lab(e) or e["rows"]
        cand = [f for f in firsts[i + 1:] if f > max(own[-1], lw[-1])]
        nf = min(cand) if cand else None
        cam = moved52(s, [si for si in range(lw[0] - PRE_W, min(lw[-1] + 20, s.sis[-1]) + 1) if si in s.rows])
        T = set(si for si, x in e["entries"].items() if x.get("transition") == 1)
        rs = dict(reasons_by.get(e["start_frame"]) or {})
        for si in T:
            if si not in rs:
                rs[si] = ("temporal_aa",)
        m = measure52(s, e, nf, tables[t], rois[t], excl - infl(own), has_record, lw, T, rs, taa)
        m["camera_moved"] = cam
        out[e["start_frame"]] = m
    return out


class Agg(object):
    def __init__(self):
        self.status = collections.Counter()
        self.release = collections.Counter()
        self.raw_release = collections.Counter()
        self.S = {n: {"ta": [], "raw": []} for n, _t in THRESH}
        self.E = {n: {"ta": [], "raw": []} for n, _t in THRESH}
        self.cs = 0
        self.ce = 0
        self.us = 0
        self.ue = 0
        self.gap = 0
        self.fb = 0
        self.runs = 0
        self.wrong = 0
        self.wrong_clean = 0
        self.mm = 0
        self.mx = 0
        self.post1 = []
        self.ce_w = collections.Counter()
        self.stats = collections.Counter()
        self.excused = 0
        self.disagree = 0
        self.fallback_events = 0
        self.part_edges = collections.Counter()
        self.part_edges_warm = collections.Counter()
        self.part_investigate = 0
        self.events = 0


def aggregate(rows):
    aggs = collections.OrderedDict()
    for t in DELIVERED:
        aggs[t] = Agg()
    for r in rows:
        t = r["type"]
        if t not in aggs:
            aggs[t] = Agg()
        a = aggs[t]
        a.events += 1
        a.disagree += r.get("disagree") or 0
        st = r.get("status")
        if st == "NOT-JUDGEABLE":
            a.status["not-judgeable"] += 1
            continue
        if r.get("path") == "m52-fallback-d":
            a.fallback_events += 1
        d = r.get("d") or {}
        if d.get("status") == "OK" and d.get("measurable") and st not in ("WARMUP", "CAMERA-MOVED"):
            if d.get("wrong_obj"):
                a.wrong += 1
                if d.get("wrong_obj_clean"):
                    a.wrong_clean += 1
            a.mm += d.get("mask_missing") or 0
            a.mx += d.get("mask_extra") or 0
        if st != "OK":
            a.status[{"WARMUP": "warm-up", "CAMERA-MOVED": "camera moved", "NOT-MEASURABLE": "not measurable",
                      "CONFOUNDED": "confounded", "CONFOUNDED-REFERENCE": "confounded reference"}.get(
                          st, "not judged (no reference or frames)")] += 1
            continue
        a.status["judged"] += 1
        for c in r.get("ce") or []:
            a.ce_w[c] += 1
        per = r["per"]
        a.release[per[RELEASE]["verdict"]] += 1
        a.raw_release[per[RELEASE]["verdict_raw"]] += 1
        a.stats.update(per[RELEASE]["stats"])
        a.excused += per[RELEASE]["excused"]
        for name, _t in THRESH:
            for e in per[name]["edges"]:
                if name == RELEASE:
                    a.runs += 1
                    a.cs += 1 if e["cs"] else 0
                    a.ce += 1 if e["ce"] else 0
                    a.us += 1 if e["us"] else 0
                    a.ue += 1 if e["ue"] else 0
                    a.gap += (1 if e["gs"] else 0) + (1 if e["ge"] else 0)
                    a.fb += 1 if e.get("fb") else 0
                if not e["cs"] and not e["us"]:
                    if e["start"] is not None:
                        a.S[name]["raw"].append(e["start"])
                    if e["start_ta"] is not None:
                        a.S[name]["ta"].append(e["start_ta"])
                if not e["ce"] and not e["ue"]:
                    if e["end"] is not None:
                        a.E[name]["raw"].append(e["end"])
                    if e["end_ta"] is not None:
                        a.E[name]["ta"].append(e["end_ta"])
        if r.get("path") == "m52":
            m = r["m52"]
            a.post1 += m.get("post1") or []
            bucket = a.part_edges_warm if m.get("warm") else a.part_edges
            for n in (m.get("part_on", 0), m.get("part_off", 0)):
                bucket[n] += 1
                if n > PARTIAL_INVESTIGATE:
                    a.part_investigate += 1
        else:
            a.post1 += d.get("post1") or []
    return aggs


def _counter_line(c, keys):
    return " | ".join("%s %d" % (k, c.get(k, 0)) for k in keys)


def _pct(v):
    return "%.3f" % v


def report(sessions_info, rows, decoder, elapsed, frames_decoded):
    lines = []
    w = lines.append
    w("LABEL SYNC CHECK  kit %s  evaluator %s" % (KIT_VERSION, EVALUATOR))
    w("method: %s" % METHOD)
    w("decoder: %s" % ("Pillow" if decoder == "pillow" else "standard library"))
    si = sessions_info
    w("sessions read %d | refused %d (%s) | duplicates skipped %d" % (
        si["read"], sum(si["refused"].values()),
        ", ".join("%s %d" % (k, v) for k, v in sorted(si["refused"].items())) or "none", si["dups"]))
    w("resolutions: %s" % (", ".join("%dx%d x%d" % (k[0], k[1], v) for k, v in sorted(si["res"].items())) or "none"))
    w("label rule: new (labelled key) %d | transition flags without labelled key %d | old (no flags; read raw) %d" % (
        si["rule"].get("new", 0), si["rule"].get("flags", 0), si["rule"].get("old", 0)))
    w("temporal anti-aliasing: on %d | off %d | not recorded %d" % (si["taa"].get(True, 0), si["taa"].get(False, 0),
                                                                     si["taa"].get(None, 0)))
    w("sessions without target masks (box used, lower confidence) %d" % si["nomask"])
    aggs = aggregate(rows)
    tot_runs = sum(a.runs for a in aggs.values())
    tot_fb = sum(a.fb for a in aggs.values())
    tot_52fb = sum(a.fallback_events for a in aggs.values())
    w("thresholds: edge-local references (084-06) on %d of %d judged run ends; %d judged on the onset reference "
      "(086-01 constants; no clean frames after the run)" % (tot_runs - tot_fb, tot_runs, tot_fb))
    w("stuck_low_mip: sharpness basis own-detrend, edge-local noise (no null session); %d event(s) judged by the "
      "086-01 path because the sharpness path lacked frames" % tot_52fb)
    w("release reading: transition-aware at 50 % of the effect; raw, 10 % and strict printed beside it")
    w("offsets: first (or last) visible frame minus first (or last) labelled frame; + means the picture lags the label")
    w("")
    for t, a in aggs.items():
        w("== %s ==" % t)
        if t in NOT_JUDGEABLE:
            w("events %d | not judgeable by this kit: %s" % (a.events, NOT_JUDGEABLE[t]))
            w("")
            continue
        w("events %d | %s" % (a.events, _counter_line(a.status, ("judged", "camera moved", "warm-up", "not measurable",
                                                                  "confounded", "confounded reference",
                                                                  "not judged (no reference or frames)"))))
        if not a.status.get("judged"):
            w("no judged event")
            w("")
            continue
        w("RELEASE (t50 transition-aware): pass %d | fail %d | censored %d      (raw: pass %d | fail %d | censored %d)" % (
            a.release.get("PASS", 0), a.release.get("FAIL", 0), a.release.get("CENSORED", 0),
            a.raw_release.get("PASS", 0), a.raw_release.get("FAIL", 0), a.raw_release.get("CENSORED", 0)))
        for name, _t in (("t50", 0), ("t10", 0), ("strict", 0)):
            w("  start %-6s transition-aware %-24s raw %s" % (name, hist(a.S[name]["ta"]), hist(a.S[name]["raw"])))
            w("  end   %-6s transition-aware %-24s raw %s" % (name, hist(a.E[name]["ta"]), hist(a.E[name]["raw"])))
        w("edges: censored start %d end %d | unresolved start %d end %d | beside an uncaptured gap %d" % (
            a.cs, a.ce, a.us, a.ue, a.gap))
        w("wrong-object (upper bound) %d event(s) | of them also changing on unlabelled frames %d" % (a.wrong, a.wrong_clean))
        w("masks: labelled frames without a mask %d | masks outside the label %d" % (a.mm, a.mx))
        if a.post1:
            p = sorted(a.post1)
            w("post-edge residual (share of the effect on the first frame after the label) median %s max %s" % (
                _pct(median(p)), _pct(p[-1])))
        st = a.stats
        w("transition frames excused %d (onset %d, after the end %d) | partial confirmed %d, flagged-invisible %d, "
          "unbacked %d | unresolved-only off frames %d" % (
              a.excused, st.get("onset_excused", 0), st.get("tail_excused", 0), st.get("partial_confirmed", 0),
              st.get("partial_invisible", 0), st.get("partial_unbacked", 0), st.get("unresolved_off", 0)))
        if t == M52:
            w("partial frames per edge %s | warm-up events %s | edges over %d partial frames %d" % (
                hist(list(a.part_edges.elements())), hist(list(a.part_edges_warm.elements())), PARTIAL_INVESTIGATE,
                a.part_investigate))
        w("labelled key disagreeing with the annotation: %d frame(s)" % a.disagree)
        w("m55 onset witness: %s" % _counter_line(a.ce_w, ("onset-on-first", "label-early", "no-change", "partial-first",
                                                             "unmeasured")))
        w("")
    w("READ BACK (release reading: transition-aware, 50 %)")
    for t, a in aggs.items():
        if t in NOT_JUDGEABLE:
            w("  %-18s events %d, not judgeable by this kit" % (t, a.events))
            continue
        if not a.status.get("judged"):
            w("  %-18s events %d, judged 0" % (t, a.events))
            continue
        w("  %-18s judged %d | start %s | end %s | wrong-object %d | censored %d | fail %d" % (
            t, a.status.get("judged", 0), hist(a.S[RELEASE]["ta"]), hist(a.E[RELEASE]["ta"]), a.wrong, a.cs + a.ce,
            a.release.get("FAIL", 0)))
    w("")
    w("frames decoded %d | seconds %.0f" % (frames_decoded, elapsed))
    return "\n".join(lines) + "\n"


def find_sessions(paths):
    found = []
    for p in paths:
        p = os.path.abspath(p)
        if os.path.isfile(os.path.join(p, "annotation.json")):
            found.append(p)
            continue
        for base, dirs, files in os.walk(p):
            dirs[:] = sorted(x for x in dirs if x not in ("Actual_Frames", "target_mask", "Video_Clip"))
            if "annotation.json" in files:
                found.append(base)
                dirs[:] = []
    return found


def refuse_reason(d):
    if not os.path.isfile(os.path.join(d, "labels.jsonl")):
        return "no labels"
    anno = jload(os.path.join(d, "annotation.json"), None)
    if not isinstance(anno, dict):
        return "unreadable annotation"
    sch = anno.get("label_schema")
    if sch is None or sch < 2:
        return "label schema 1"
    fr = os.path.join(d, "Actual_Frames")
    if not os.path.isdir(fr):
        return "no frames"
    names = os.listdir(fr)
    if any(n.lower().endswith((".jpg", ".jpeg")) for n in names) and not any(n.lower().endswith(".png") for n in names):
        return "jpeg frames"
    pngs = [n for n in names if n.lower().endswith(".png")]
    if not pngs:
        return "no frames"
    try:
        png_parse(os.path.join(fr, sorted(pngs)[0]), need_data=False)
    except (PngError, OSError, struct.error):
        return "unsupported png"
    return None


def run(paths, decoder, types=None):
    t0 = time.time()
    info = dict(read=0, refused=collections.Counter(), dups=0, res=collections.Counter(), rule=collections.Counter(),
                taa=collections.Counter(), nomask=0)
    rows = []
    seen = set()
    frames = 0
    for d in find_sessions(paths):
        why = refuse_reason(d)
        if why:
            info["refused"][why] += 1
            continue
        anno = jload(os.path.join(d, "annotation.json"), {}) or {}
        sid = anno.get("session_id") or os.path.basename(d)
        if sid in seen:
            info["dups"] += 1
            continue
        seen.add(sid)
        s, rule, taa, out = analyse_session(d, decoder, types)
        info["read"] += 1
        info["rule"][rule] += 1
        info["taa"][taa] += 1
        if not os.path.isdir(os.path.join(d, "target_mask")):
            info["nomask"] += 1
        if s.size:
            info["res"][s.size] += 1
        frames += s.decoded + s.decoded_partial
        rows += out
    return info, rows, time.time() - t0, frames


def choose_decoder(force_stdlib):
    if force_stdlib:
        return "stdlib"
    return "pillow" if pillow() is not None else "stdlib"


def _png_chunk(t, body):
    return struct.pack(">I", len(body)) + t + body + struct.pack(">I", zlib.crc32(t + body) & 0xffffffff)


def _paeth_pred(a, b, c):
    pp = a + b - c
    pa, pb, pc = abs(pp - a), abs(pp - b), abs(pp - c)
    return a if (pa <= pb and pa <= pc) else (b if pb <= pc else c)


def _filter_row(ft, row, prev, bpp):
    if ft == 0:
        return row
    left = bytes(bpp) + row[:-bpp]
    if ft == 1:
        pred = left
    elif ft == 2:
        pred = prev
    elif ft == 3:
        pred = bytes(map(operator.rshift, map(_add, left, prev), itertools.repeat(1)))
    else:
        upleft = bytes(bpp) + prev[:-bpp]
        pred = bytes(map(_paeth_pred, left, prev, upleft))
    return bytes(map(_AND255, map(_sub, row, pred)))


def write_png(path, w, h, rows, ct):
    bpp = CHANNELS[ct]
    raw = bytearray()
    prev = bytes(w * bpp)
    for y, row in enumerate(rows):
        ft = (y * 7 + 3) % 5
        raw.append(ft)
        raw += _filter_row(ft, row, prev, bpp)
        prev = row
    data = PNG_SIG + _png_chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, ct, 0, 0, 0))
    data += _png_chunk(b"IDAT", zlib.compress(bytes(raw), 6)) + _png_chunk(b"IEND", b"")
    with open(path, "wb") as fh:
        fh.write(data)


ST_W, ST_H = 128, 96
ST_OBJ = (80, 30, 116, 70)
ST_WRONG = (4, 50, 30, 80)
ST_MV = 211


_ST_BG = {}
_ST_SEG = {}


def _st_bg(variant):
    rows = _ST_BG.get(variant)
    if rows is None:
        rows = []
        for y in range(ST_H):
            row = bytearray(3 * ST_W)
            for x in range(ST_W):
                v = 96 + ((x * 7 + y * 13) % 48) + (1 if ((x * 31 + y * 17 + variant * 7) % 11) == 0 else 0)
                row[3 * x] = row[3 * x + 1] = row[3 * x + 2] = v
            rows.append(bytes(row))
        _ST_BG[variant] = rows
    return rows


def _st_seg(kind, f, drift, variant, y):
    key = (kind, f, drift, variant if kind == "hide" else -1, y if kind in ("hide", "blur") else -1)
    seg = _ST_SEG.get(key)
    if seg is not None:
        return seg
    ox0, _oy0, ox1, _oy1 = ST_OBJ
    out = bytearray(3 * (ox1 - ox0))
    bgrow = _st_bg(variant)[y]
    for i, x in enumerate(range(ox0, ox1)):
        if kind == "hide":
            bg = bgrow[3 * x]
            c = [int(round(o + f * (bg - o))) for o in (200, 190, 60)]
        elif kind == "tex":
            c = [min(255, int(round(o + f * (m - o))) + drift) for o, m in zip((150, 150, 150), (255, 0, 255))]
        else:
            chk = 40 if ((x // 2 + y // 2) % 2) else 220
            v = int(round(chk + f * (130 - chk)))
            c = [v, v, v]
        out[3 * i:3 * i + 3] = bytes(max(0, min(255, q)) for q in c)
    seg = bytes(out)
    _ST_SEG[key] = seg
    return seg


def _st_frame(si, spec, state):
    ox0, oy0, ox1, oy1 = ST_OBJ
    kind = spec["kind"]
    f = state.get("f", 0.0)
    drift = state.get("drift", 0)
    variant = si % 11
    bg = _st_bg(variant)
    rows = []
    for y in range(ST_H):
        row = bg[y]
        if oy0 <= y < oy1:
            row = row[:3 * ox0] + _st_seg(kind, f, drift, variant, y) + row[3 * ox1:]
        if state.get("wrong") and ST_WRONG[1] <= y < ST_WRONG[3]:
            row = row[:3 * ST_WRONG[0]] + bytes(3 * (ST_WRONG[2] - ST_WRONG[0])) + row[3 * ST_WRONG[2]:]
        rows.append(row)
    return rows


def _to_ct(rows, ct):
    if ct == 2:
        return rows
    out = []
    for r in rows:
        n = len(r) // 3
        o = bytearray(4 * n)
        o[0::4] = r[0::3]
        o[1::4] = r[1::3]
        o[2::4] = r[2::3]
        o[3::4] = b"\xff" * n
        out.append(bytes(o))
    return out


def st_make(root, name, spec):
    d = os.path.join(root, name)
    os.makedirs(os.path.join(d, "Actual_Frames"))
    os.makedirs(os.path.join(d, "target_mask"))
    W, H = spec.get("W", ST_W), spec.get("H", ST_H)
    n = spec["n"]
    ct = spec.get("ct", 2)
    typ = spec["type"]
    lab_id = "blinking" if typ == "blink" else typ
    labels = []
    anomalies = []
    fire_all = {}
    evs = spec["events"]
    for k, ev in enumerate(evs):
        L = ev["L"]
        fire = set(range(min(L) - ev.get("fire_pre", 1), max(L) + 1)) | set(ev.get("flags", {}).keys())
        fire_all[k] = fire
        anomalies.append({"anomaly_type": typ, "anomaly_subtype": typ, "injected_frames": {"frame_indices": sorted(L)},
                          "affected_frames": {"frame_indices": sorted(L)}, "manifested": True,
                          "affected_objects": {"nodes": [{"name": "T%d" % k}]}})
    gap_at = spec.get("gap_before")
    written = {}
    obj_mask = [bytes(ST_MV if (ST_OBJ[0] <= x < ST_OBJ[2] and ST_OBJ[1] <= y < ST_OBJ[3]) else 0 for x in range(W))
                for y in range(H)]
    for si in range(n):
        state = {"f": 0.0}
        for k, ev in enumerate(evs):
            pf = ev.get("pixels", {})
            if si in pf:
                state["f"] = pf[si]
        if spec.get("drift_from") is not None and si >= spec["drift_from"]:
            state["drift"] = (si - spec["drift_from"]) * spec.get("drift_step", 3)
        if spec.get("wrong") and any(si in ev["L"] for ev in evs):
            state["wrong"] = True
        rows = _st_frame(si, spec, state)
        p = os.path.join(d, "Actual_Frames", "frame_%05d.png" % si)
        write_png(p, W, H, _to_ct(rows, ct), ct)
        written[si] = rows
        entries = []
        present = False
        mask_rows = None
        for k, ev in enumerate(evs):
            if si not in fire_all[k]:
                continue
            L = set(ev["L"])
            ent = {"id": lab_id, "target_name": "T%d" % k, "start_frame": 5000 + k, "bbox_valid": True,
                   "bbox_px": [ST_OBJ[0], ST_OBJ[1], ST_OBJ[2] - ST_OBJ[0], ST_OBJ[3] - ST_OBJ[1]],
                   "mask_value": ST_MV, "target_pixels": -1}
            if spec.get("rule", "new") == "new":
                ent["labelled"] = si in L
            fl = ev.get("flags", {}).get(si)
            if fl is not None:
                ent["transition"] = 1
                if fl:
                    ent["transition_reason"] = list(fl)
            if typ == M52:
                rec = ev.get("record", {}).get(si, "held" if si in L else "baseline")
                ent["stuck_mip.held"] = si in L
                ent["stuck_mip.render_state"] = "held" if si in L else "none"
                ent["stuck_mip.textures"] = [{"name": "T_N", "forced_mips": 7, "baseline_mips": 11}]
                ent["stuck_mip.render_textures"] = [{"name": "T_N", "baseline_mips": 11, "held_level_mips": 7, "level": rec}]
            transition_only = fl is not None and si not in L and si > max(L)
            if not transition_only:
                present = True
            entries.append(ent)
            if si in L and si not in ev.get("mask_missing", ()):
                mask_rows = obj_mask
        if mask_rows is not None:
            write_png(os.path.join(d, "target_mask", "frame_%05d.png" % si), W, H, mask_rows, 0)
        fi = 1000 + si + (5 if (gap_at is not None and si >= gap_at) else 0)
        origin = [100.0, 0.0, 50.0]
        if spec.get("move_at") is not None and si >= spec["move_at"]:
            origin = [100.0 + (si - spec["move_at"]) * 2.0, 0.0, 50.0]
        row = {"session_index": si, "frame_index": fi, "t": si / 30.0, "image": "Actual_Frames/frame_%05d.png" % si,
               "width": W, "height": H, "anomaly_present": present, "anomalies": entries,
               "mask_file": ("target_mask/frame_%05d.png" % si) if mask_rows is not None else None,
               "view": {"origin": origin, "rot": [0, 0, 0], "fovDeg": 90, "aspect": W / float(H), "valid": True}}
        labels.append(row)
    random_order = sorted(labels, key=lambda r: (r["session_index"] * 7919) % 101)
    with open(os.path.join(d, "labels.jsonl"), "w", encoding="utf-8") as fh:
        for r in random_order:
            fh.write(json.dumps(r) + "\n")
    with open(os.path.join(d, "annotation.json"), "w", encoding="utf-8") as fh:
        json.dump({"label_schema": 2, "session_id": "st_" + name, "anomalies": anomalies}, fh)
    summ = {}
    if spec.get("rule", "new") in ("new", "flags"):
        summ.update({"label_temporal_aa": bool(spec.get("taa", False)), "label_transition_on_frames": 3,
                     "label_transition_off_frames": 8, "label_transition_hide_frames": 1})
    if spec.get("rule", "new") == "new":
        summ["label_labelled_rule"] = "annotation_membership_per_policy_v1"
    with open(os.path.join(d, "run_summary.json"), "w", encoding="utf-8") as fh:
        json.dump(summ, fh)
    return d, written


def _full(L, v=1.0):
    return {si: v for si in L}


def st_cases():
    L = list(range(40, 48))
    cases = []

    def tex(name, pixels, labels=None, **kw):
        ev = {"L": labels if labels is not None else L, "pixels": pixels}
        ev.update(kw.pop("ev", {}))
        spec = {"type": "corrupted_texture", "kind": "tex", "n": 72, "events": [ev]}
        spec.update(kw)
        cases.append((name, spec))

    tex("exact", _full(L))
    tex("onset_late_1", _full(range(40, 48)), labels=list(range(41, 48)))
    tex("onset_early_1", _full(range(41, 48)), labels=list(range(40, 48)))
    tex("offset_early_1", _full(range(40, 48)), labels=list(range(40, 47)))
    tex("offset_late_1", _full(range(40, 47)), labels=list(range(40, 48)))
    tex("delayed_swap_3", _full(range(43, 48)), labels=list(range(40, 48)))
    tex("slow_drift", _full(L), drift_from=48, drift_step=4)
    tex("missing_mask", _full(L), ev={"mask_missing": (42,)})
    tex("moving_camera", _full(L), move_at=36)
    tex("gap_exact", _full(L), gap_before=40)
    tex("gap_late_1", _full(range(40, 48)), labels=list(range(41, 48)), gap_before=41)
    tex("wrong_object", _full(L), wrong=True)
    tex("old_rule_late_1", _full(range(40, 48)), labels=list(range(41, 48)), rule="old")
    smear = _full(L)
    smear.update({48: 0.7, 49: 0.35, 50: 0.1})
    flags = {si: ("temporal_aa",) for si in range(48, 56)}
    flags.update({si: ("temporal_aa",) for si in (40, 41, 42)})
    tex("taa_smear_flagged", smear, taa=True, ev={"flags": flags})
    wrongflag = {39: ("temporal_aa",), 48: ("temporal_aa",)}
    tex("late_1_flag_covering", _full(range(39, 47)), labels=list(range(40, 49)), taa=True, ev={"flags": wrongflag})
    ghost = {si: 1.0 for si in L}
    ghost[48] = 0.08
    cases.append(("ghost_8pct", {"type": "blink", "kind": "hide", "n": 72, "events": [{"L": L, "pixels": ghost}]}))
    cases.append(("cc", {"type": "camera_clipping", "kind": "tex", "n": 60, "events": [{"L": list(range(40, 44)),
                                                                                         "pixels": {}}]}))
    L52 = list(range(60, 70))

    def m52(name, pixels, flags=None, record=None, taa=False, labels=None):
        ev = {"L": labels or L52, "pixels": pixels, "flags": flags or {}, "record": record or {}, "fire_pre": 1}
        spec = {"type": M52, "kind": "blur", "n": 190, "events": [ev], "taa": taa}
        cases.append((name, spec))

    m52("m52_exact", _full(L52))
    p = _full(L52)
    p.update({60: 0.3, 61: 0.3})
    m52("m52_partial_confirmed", p, flags={60: ("partial",), 61: ("partial",)})
    p = _full(L52)
    p[60] = 0.0
    m52("m52_partial_invisible", p, flags={60: ("partial",)}, record={60: "held"})
    m52("m52_partial_unbacked", p, flags={60: ("partial",)}, record={60: "baseline"})
    m52("m52_unresolved_only", p, flags={60: ("unresolved",)})
    p = _full(L52)
    p.update({70: 0.7, 71: 0.35, 72: 0.1})
    fl = {si: ("temporal_aa",) for si in range(70, 78)}
    fl.update({60: ("temporal_aa",), 61: ("temporal_aa",), 62: ("temporal_aa",)})
    m52("m52_taa_smear", p, flags=fl, taa=True)
    return cases


def _one(rows, typ):
    r = [x for x in rows if x["type"] == typ]
    return r[0] if r else None


def _edge(row, thr="t50"):
    if row is None or "per" not in row:
        return None
    return row["per"][thr]["edges"][0]


def st_expect():
    def verdict(row, thr="t50"):
        return row["per"][thr]["verdict"] if row and "per" in row else (row or {}).get("status")

    def raw(row, thr="t50"):
        return row["per"][thr]["verdict_raw"] if row and "per" in row else (row or {}).get("status")

    X = []

    def add(case, label, fn, doctored=False):
        X.append((case, label, fn, doctored))

    add("exact", "exact labels PASS at 0/0", lambda r: verdict(r) == "PASS" and _edge(r)["start_ta"] == 0 and _edge(r)["end_ta"] == 0)
    add("onset_late_1", "label late by 1 FAILS, start -1", lambda r: verdict(r) == "FAIL" and _edge(r)["start"] == -1, True)
    add("onset_early_1", "label early by 1 FAILS, start +1", lambda r: verdict(r) == "FAIL" and _edge(r)["start"] == 1, True)
    add("offset_early_1", "label ends 1 early FAILS, end +1", lambda r: verdict(r) == "FAIL" and _edge(r)["end"] == 1, True)
    add("offset_late_1", "label ends 1 late FAILS, end -1", lambda r: verdict(r) == "FAIL" and _edge(r)["end"] == -1, True)
    add("delayed_swap_3", "effect 3 frames late FAILS, start +3", lambda r: verdict(r) == "FAIL" and _edge(r)["start"] == 3, True)
    add("ghost_8pct", "8 % reappear ghost: 50 % end 0 PASS, 10 % end 0, strict end +1",
        lambda r: verdict(r) == "PASS" and _edge(r)["end"] == 0 and _edge(r, "t10")["end"] == 0 and _edge(r, "strict")["end"] == 1)
    add("slow_drift", "slow drift after the event: end censored, not failed",
        lambda r: verdict(r) == "CENSORED" and _edge(r)["ce"])
    add("missing_mask", "one labelled frame without a mask is counted",
        lambda r: r["d"]["mask_missing"] == 1 and verdict(r) == "PASS")
    add("moving_camera", "moving camera is not judged", lambda r: r["status"] == "CAMERA-MOVED")
    add("gap_exact", "gap beside the onset, exact labels: PASS, gap counted",
        lambda r: verdict(r) == "PASS" and _edge(r)["gs"])
    add("gap_late_1", "gap beside the onset, label late by 1: FAIL (a gap never excuses)",
        lambda r: verdict(r) == "FAIL" and _edge(r)["start"] == -1, True)
    add("wrong_object", "second object changing on labelled frames counted as wrong-object",
        lambda r: r["d"]["wrong_obj"] and not r["d"]["wrong_obj_clean"])
    add("exact", "exact case has no wrong-object", lambda r: not r["d"]["wrong_obj"])
    add("old_rule_late_1", "old-rule session read raw, late by 1 FAILS",
        lambda r: verdict(r) == "FAIL" and raw(r) == "FAIL" and r["rule"] == "old", True)
    add("taa_smear_flagged", "flagged TAA smear: transition-aware PASS end 0, raw end +1",
        lambda r: verdict(r) == "PASS" and _edge(r)["end_ta"] == 0 and _edge(r)["end"] == 1 and raw(r) == "FAIL")
    add("late_1_flag_covering", "label late by 1 with the flag wrongly covering it FAILS at 50 %",
        lambda r: verdict(r) == "FAIL", True)
    add("cc", "camera_clipping is not judgeable", lambda r: r["status"] == "NOT-JUDGEABLE" and "per" not in r)
    add("m52_exact", "stuck_low_mip exact PASS 0/0", lambda r: verdict(r) == "PASS" and _edge(r)["start"] == 0 and _edge(r)["end"] == 0)
    add("m52_partial_confirmed", "partial frames above the noise band: transition-aware PASS, raw start +2",
        lambda r: verdict(r) == "PASS" and _edge(r)["start"] == 2 and r["per"]["t50"]["stats"].get("partial_confirmed") == 2)
    add("m52_partial_invisible", "partial frame within the noise band with the record: counted, PASS",
        lambda r: verdict(r) == "PASS" and r["per"]["t50"]["stats"].get("partial_invisible") == 1)
    add("m52_partial_unbacked", "partial flag within the noise band without the record FAILS",
        lambda r: verdict(r) == "FAIL", True)
    add("m52_unresolved_only", "an off frame backed only by unresolved FAILS", lambda r: verdict(r) == "FAIL", True)
    add("m52_taa_smear", "stuck_low_mip TAA tail flagged: transition-aware PASS, raw end > 0",
        lambda r: verdict(r) == "PASS" and (_edge(r)["end"] or 0) > 0 and _edge(r)["end_ta"] == 0)
    return X


def _canon(rows):
    out = []
    for r in rows:
        c = {k: v for k, v in r.items() if k not in ("d", "m52")}
        if "d" in r:
            c["d"] = {k: v for k, v in r["d"].items() if k in ("status", "mask_missing", "mask_extra", "wrong_obj", "wrong_obj_clean", "post1")}
            if "per" in r["d"]:
                c["dper"] = r["d"]["per"]
        out.append(c)
    return json.dumps(out, sort_keys=True, default=str)


def selftest(force_stdlib=False):
    t0 = time.time()
    root = tempfile.mkdtemp(prefix="label_sync_selftest_")
    ok_all = True
    lines = []
    try:
        cases = st_cases()
        made = {}
        pix = {}
        for name, spec in cases:
            made[name], pix[name] = st_make(root, name, spec)
        decs = ["stdlib"]
        if not force_stdlib and pillow() is not None:
            decs.append("pillow")
        dec_ok = True
        checked = 0
        for name in ("exact", "ghost_8pct", "m52_exact"):
            d = made[name]
            for si in (0, 41, 60):
                p = os.path.join(d, "Actual_Frames", "frame_%05d.png" % si)
                if not os.path.isfile(p):
                    continue
                for dec in decs:
                    w, h, ct, plte, nc, rows = decode_native(p, dec)
                    if rows_to_rgb(rows, ct, plte, nc) != pix[name][si]:
                        dec_ok = False
                    checked += 1
        rgba = os.path.join(root, "rgba.png")
        src = _st_frame(3, {"kind": "tex"}, {"f": 0.5})
        write_png(rgba, ST_W, ST_H, _to_ct(src, 6), 6)
        for dec in decs:
            w, h, ct, plte, nc, rows = decode_native(rgba, dec)
            if rows_to_rgb(rows, ct, plte, nc) != src:
                dec_ok = False
            w, h, ct, plte, nc, rows = decode_native(rgba, dec, 40, 50)
            if rows_to_rgb(rows, ct, plte, nc) != [r[:150] for r in src[:40]]:
                dec_ok = False
            checked += 2
        lines.append("SELFTEST %-66s %s" % ("PNG decoder reproduces the written pixels (all 5 row filters, RGB/RGBA/grey, partial rows; %d decodes)" % checked,
                                          "ok" if dec_ok else "*** WRONG ***"))
        ok_all = ok_all and dec_ok
        results = {}
        for dec in decs:
            res = {}
            for name, _spec in cases:
                _s, _rule, _taa, rows = analyse_session(made[name], dec)
                res[name] = rows
            results[dec] = res
        base = results["stdlib"]
        doctored_passed = []
        for case, label, fn, doctored in st_expect():
            rows = base[case]
            typ = rows[0]["type"] if rows else None
            r = _one(rows, typ)
            try:
                good = bool(fn(r))
            except Exception:
                good = False
            ok_all = ok_all and good
            if doctored and not good and r is not None and r.get("per") and r["per"]["t50"]["verdict"] == "PASS":
                doctored_passed.append(case)
            lines.append("SELFTEST %-66s %s" % (label, "ok" if good else "*** WRONG ***"))
        if len(decs) == 2:
            same = all(_canon(results["stdlib"][n]) == _canon(results["pillow"][n]) for n, _s in cases)
            lines.append("SELFTEST %-66s %s" % ("Pillow and standard-library decoders give identical numbers (%d sessions)" % len(cases),
                                              "ok" if same else "*** WRONG ***"))
            ok_all = ok_all and same
        else:
            lines.append("SELFTEST %-66s %s" % ("Pillow not installed: decoder-identity check skipped", "skipped"))
        info, rows, el, fr = run([made[n] for n, _s in cases], "stdlib")
        text = report(info, rows, "stdlib", el, fr)
        clean = not re.search(r"[\\/]|\.png|\.json|session_|frame_\d|\bT\d\b|st_|label_sync_selftest", text)
        lines.append("SELFTEST %-66s %s" % ("report prints numbers only (no path, folder or object name)", "ok" if clean else "*** WRONG ***"))
        ok_all = ok_all and clean
        if doctored_passed:
            ok_all = False
            lines.append("SELFTEST *** THE KIT PASSED A DOCTORED LABEL (%d case(s)) - DO NOT USE ITS NUMBERS ***" % len(doctored_passed))
    finally:
        shutil.rmtree(root, ignore_errors=True)
    for l in lines:
        print(l)
    print("SELFTEST %d check(s): %s  (%.0f s)" % (len(lines), "OK" if ok_all else "FAILED", time.time() - t0))
    return 0 if ok_all else 1


def main(argv=None):
    ap = argparse.ArgumentParser(description="Office label-sync check: numbers only.")
    ap.add_argument("paths", nargs="*", help="session folders, or folders that contain sessions")
    ap.add_argument("--selftest", action="store_true", help="run the known-answer self test first; it must pass")
    ap.add_argument("--stdlib", action="store_true", help="use the standard-library PNG decoder even if Pillow is installed")
    ap.add_argument("--out", help="also write the numbers to this text file")
    a = ap.parse_args(argv)
    if a.selftest:
        return selftest(a.stdlib)
    if not a.paths:
        ap.print_help()
        return 2
    dec = choose_decoder(a.stdlib)
    info, rows, elapsed, frames = run(a.paths, dec)
    text = report(info, rows, dec, elapsed, frames)
    sys.stdout.write(text)
    if a.out:
        with open(a.out, "w", encoding="utf-8") as fh:
            fh.write(text)
    return 0 if info["read"] else 2


if __name__ == "__main__":
    sys.exit(main())
