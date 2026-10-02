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

KIT_VERSION = "1.4"
EVALUATOR = "090-10e"
METHOD = ("086-01 per-frame change on the target silhouette; 084-06 edge-local references, stuck_low_mip "
          "sharpness path and transition-aware gate; 084-07 labelled and partial rules; 090-07 effect_interrupted "
          "run edges read against the picture at the interruption; 090-09 a labelled run with an image it cannot "
          "read leaves its event unjudged unless a judged run fails, no anti-aliasing excuse on an interrupted or "
          "nanite frame, nanite frames left out and the run end before them censored; 090-10 a labels row missing "
          "inside a labelled run or between two runs leaves those runs unjudged, every PNG is checked whole before "
          "either decoder reads it, capture_unpaired frames are left out with the run edges beside them censored, "
          "and a session made only of them is refused; 090-10e a pie_end_settle frame (the first frame after a "
          "labelled run of a fire-window type in a Play-In-Editor capture) is left out of its event and is never a "
          "reference or an anti-aliasing excuse, the run end is still judged on the next frame, which must read "
          "clean, and the flag outside a PIE capture refuses the session while one on a labelled frame, on another "
          "type or anywhere but the first frame after a run fails the event")

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
INTERRUPT_REASON = "effect_interrupted"
NANITE_REASON = "nanite_unmaskable"
UNPAIRED_REASON = "capture_unpaired"
PIE_REASON = "pie_end_settle"
PIE_TYPES = ("missing_texture", "corrupted_texture", "uv_corruption", "normal_corruption", "lighting_mismatch",
             "lod_corruption", "null_effect", "solid_swap", "time_dilation")
NO_AA_EXCUSE = (INTERRUPT_REASON, NANITE_REASON, UNPAIRED_REASON, PIE_REASON)
KNOWN_REASONS = AA_ONLY_REASONS + ("partial", "camera_clipping_unconfirmed", "unresolved", INTERRUPT_REASON,
                                   NANITE_REASON, UNPAIRED_REASON, PIE_REASON)
PIE_OUTSIDE = "pie_end_settle outside a PIE capture"
PIE_LABELLED = "pie_end_settle on a labelled frame"
PIE_TYPE = "pie_end_settle on a type without a fire window"
PIE_PLACE = "pie_end_settle not on the first frame after a labelled run"
ROW_MISSING = "labels row missing"
LABELLED_UNPAIRED = "labelled frame unpaired"
SYNC_ONLY = "sync-path capture: unsupported for delivery"

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
REF_AMENDMENT = True
DEBUG = False
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
DECODE_ERRORS = (PngError, OSError, ValueError, SyntaxError, EOFError, zlib.error, struct.error, IndexError, KeyError)
_PIL = None
_PIL_TRIED = False
_PNG_OK = {}


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


def png_check(path):
    with open(path, "rb") as fh:
        data = fh.read()
    if data[:8] != PNG_SIG:
        raise PngError("not a PNG")
    n = len(data)
    pos = 8
    ihdr = None
    plte = None
    idat = []
    ended = False
    crc32 = zlib.crc32
    unpack = struct.unpack_from
    while pos < n:
        if pos + 12 > n:
            raise PngError("invalid PNG: a chunk is cut short")
        length, ctype = unpack(">I4s", data, pos)
        end = pos + 12 + length
        if end > n:
            raise PngError("invalid PNG: a chunk runs past the end of the file")
        body = data[pos + 8:end - 4]
        if crc32(body, crc32(ctype)) != unpack(">I", data, end - 4)[0]:
            raise PngError("invalid PNG: chunk CRC mismatch")
        if ihdr is None:
            if ctype != b"IHDR" or length != 13:
                raise PngError("invalid PNG: IHDR is not the first chunk")
            ihdr = struct.unpack(">IIBBBBB", body)
        elif ctype == b"IHDR":
            raise PngError("invalid PNG: a second IHDR")
        elif ctype == b"PLTE":
            plte = body
        elif ctype == b"IDAT":
            idat.append(body)
        elif ctype == b"IEND":
            ended = True
            pos = end
            break
        pos = end
    if not ended:
        raise PngError("invalid PNG: no IEND")
    if pos != n:
        raise PngError("invalid PNG: bytes after IEND")
    w, h, depth, ct, comp, filt, inter = ihdr
    if depth != 8 or inter != 0 or ct not in CHANNELS or comp != 0 or filt != 0:
        raise PngError("unsupported PNG")
    if not w or not h:
        raise PngError("invalid PNG: empty image")
    if ct == 3 and plte is None:
        raise PngError("palette PNG without a palette")
    if not idat:
        raise PngError("invalid PNG: no image data")
    stride = 1 + w * CHANNELS[ct]
    need = h * stride
    dobj = zlib.decompressobj()
    try:
        raw = dobj.decompress(b"".join(idat), need + 1)
    except zlib.error:
        raise PngError("invalid PNG: image data does not inflate")
    if len(raw) > need:
        raise PngError("invalid PNG: more image data than the image holds")
    if not dobj.eof:
        raise PngError("invalid PNG: image data stream does not end")
    if len(raw) < need:
        raise PngError("invalid PNG: image data shorter than the image")
    if dobj.unused_data:
        raise PngError("invalid PNG: bytes after the image data stream")
    if max(raw[0::stride]) > 4:
        raise PngError("invalid PNG: bad row filter")
    return (w, h, ct, plte), raw


def png_valid(path, keep=False):
    st = os.stat(path)
    key = (path, st.st_size, st.st_mtime_ns)
    hit = _PNG_OK.get(key)
    if isinstance(hit, str):
        raise PngError(hit)
    if hit is not None and not keep:
        return hit, None
    try:
        info, raw = png_check(path)
    except PngError as ex:
        _PNG_OK[key] = str(ex)
        raise
    _PNG_OK[key] = info
    return info, raw


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
        (w, h, ct, plte), _raw = png_valid(path)
        want = {0: "L", 2: "RGB", 3: "P", 4: "LA", 6: "RGBA"}[ct]
        try:
            with pillow().open(path) as im:
                if im.mode != want or im.size != (w, h):
                    raise PngError("decoder mode mismatch")
                data = im.tobytes()
        except PngError:
            raise
        except DECODE_ERRORS as ex:
            raise PngError("invalid PNG: the decoder refused it (%s)" % type(ex).__name__)
        ch = CHANNELS[ct]
        nrows = h if max_rows is None else max(0, min(h, max_rows))
        ncols = w if max_cols is None else max(0, min(w, max_cols))
        rs = w * ch
        nb = ncols * ch
        rows = [data[y * rs:y * rs + nb] for y in range(nrows)]
        return w, h, ct, plte, ncols, rows
    (w, h, ct, plte), buf = png_valid(path, True)
    ch = CHANNELS[ct]
    stride = 1 + w * ch
    nrows = h if max_rows is None else max(0, min(h, max_rows))
    ncols = w if max_cols is None else max(0, min(w, max_cols))
    nb = ncols * ch
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


def _has_unpaired(rs):
    return isinstance(rs, list) and any(str(q) == UNPAIRED_REASON for q in rs)


def row_unpaired(r):
    if r.get("capture_unpaired") or _has_unpaired(r.get("transition_reason")):
        return True
    return any(_has_unpaired(x.get("transition_reason")) for x in (r.get("anomalies") or []) if isinstance(x, dict))


def _has_pie(rs):
    return isinstance(rs, list) and any(str(q) == PIE_REASON for q in rs)


def pie_outside(d):
    summ = jload(os.path.join(d, "run_summary.json"), None)
    if isinstance(summ, dict) and summ.get("pie_end_settle_active") is True:
        return False
    try:
        with open(os.path.join(d, "labels.jsonl"), "r", encoding="utf-8-sig") as fh:
            for line in fh:
                if PIE_REASON not in line:
                    continue
                try:
                    r = json.loads(line)
                except ValueError:
                    continue
                if not isinstance(r, dict):
                    continue
                if _has_pie(r.get("transition_reason")) or any(
                        isinstance(x, dict) and _has_pie(x.get("transition_reason")) for x in (r.get("anomalies") or [])):
                    return True
    except OSError:
        return False
    return False


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
        self.pie_active = isinstance(self.summary, dict) and self.summary.get("pie_end_settle_active") is True
        self.ce_rows = [r for r in read_jsonl(os.path.join(d, "change_evidence.jsonl")) if r.get("kind") == "pair"]
        self.sis = sorted(self.rows)
        self.unpaired = set(si for si, r in self.rows.items() if row_unpaired(r))
        self.invalid = set()
        self._full = collections.OrderedDict()
        self._mask = collections.OrderedDict()
        self.decoded = 0
        self.decoded_partial = 0
        self.size = None

    def bad_png(self, si, ex):
        if str(ex).startswith("invalid PNG"):
            self.invalid.add(si)

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
            except DECODE_ERRORS as ex:
                self.bad_png(si, ex)
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
        except DECODE_ERRORS as ex:
            self.bad_png(si, ex)
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
            except DECODE_ERRORS:
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
                  T=set(), reasons={}, mv={}, bbox={}, disagree=0, has_reason=False, I=set(), N=set(), U=set(),
                  P=set(), pie_bad=[], unknown=0)
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
                                ev["unknown"] += sum(1 for q in ev["reasons"][si] if q not in KNOWN_REASONS)
                                if si not in ev["Lset"] and si not in s.unpaired:
                                    if NANITE_REASON in ev["reasons"][si]:
                                        ev["N"].add(si)
                                    elif INTERRUPT_REASON in ev["reasons"][si]:
                                        ev["I"].add(si)
                        ev["mv"][si] = x.get("mask_value")
                        ev["bbox"][si] = x.get("bbox_px")
                        if "labelled" in x and bool(x.get("labelled")) != (si in ev["Lset"]):
                            ev["disagree"] += 1
                        break
        for si in ev["T"]:
            if si in s.unpaired:
                rs = ev["reasons"].get(si, ())
                if UNPAIRED_REASON not in rs:
                    ev["reasons"][si] = (UNPAIRED_REASON,) + tuple(rs)
            elif si not in ev["reasons"]:
                ev["reasons"][si] = infer_reasons(si, ev, typ)
        ends = set(b + 1 for _a, b in ev["runs"])
        for si in sorted(ev["T"]):
            if PIE_REASON not in ev["reasons"].get(si, ()):
                continue
            lab = bool((ev["entries"].get(si) or {}).get("labelled")) or si in ev["Lset"]
            bad = [w for w, hit in ((PIE_OUTSIDE, not s.pie_active), (PIE_TYPE, typ not in PIE_TYPES),
                                    (PIE_LABELLED, lab), (PIE_PLACE, not lab and si not in ends)) if hit]
            for w in bad:
                if w not in ev["pie_bad"]:
                    ev["pie_bad"].append(w)
            if not bad and si not in s.unpaired and si not in ev["N"]:
                ev["P"].add(si)
                ev["I"].discard(si)
        near = set(ev["entries"]) | ev["Lset"]
        for a, b in ev["runs"]:
            near.update((a - 1, b + 1))
        ev["U"] = s.unpaired & near
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
        onset_taa = "temporal_aa" in rs and si <= (a + b) / 2.0 and not (set(rs) & set(NO_AA_EXCUSE))
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
        if rs & set(NO_AA_EXCUSE) or not (rs & set(AA_ONLY_REASONS)):
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


def event_verdict(measurable, confounded, fails, unjudged, cen_any):
    if measurable and not confounded and fails:
        return "FAIL"
    if unjudged:
        return "UNJUDGED"
    if not measurable:
        return "NOT-MEASURABLE"
    if confounded:
        return "CONFOUNDED"
    return "CENSORED" if cen_any else "PASS"


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
    lo, hi = L[0], L[-1]
    tail = max([x - hi for x in Ts if x > hi] or [0])
    for si in range(lo - REF_BACK - 30, hi + SPAN_PAD + tail + 6):
        for x in ((s.rows.get(si) or {}).get("anomalies") or []):
            if not (x.get("start_frame") == ev["start_frame"] and ent_type(x) == ev["type"]
                    and (ev["node"] is None or x.get("target_name") == ev["node"])):
                other.add(si)
                break
    other -= mine
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
    Us = s.unpaired

    def clean(si):
        if si not in s.rows or si in Ls or si in Ts or si in other or si in Us:
            return False
        return not (s.rows[si].get("anomaly_present") and si not in mine)

    def pre_clean(si):
        return clean(si) and si not in ev["entries"]

    prev = [x for x in other if x < lo]
    nxt_o = [x for x in other if x > hi]
    prev_last = max(prev) if prev else None
    if not REF_AMENDMENT:
        refc = [si for si in range(lo - REF_BACK, lo) if pre_clean(si)]
        if len(refc) < 3:
            refc = [si for si in range(lo - 30, lo) if pre_clean(si)][-8:]
        if len(refc) < 2:
            res["status"] = "NO-REFERENCE"
            return res
        prev_last = None
    start = lo - REF_BACK if prev_last is None else max(lo - REF_BACK, prev_last + PREV_SETTLE + 1)
    if REF_AMENDMENT:
        refc = [si for si in range(start, lo) if pre_clean(si)]
    if REF_AMENDMENT and len(refc) < REF_MIN:
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
    Is = ev["I"]
    Ns = ev["N"]
    Ps = ev["P"]
    why_unj = {}
    pre_ref = []

    def shows_effect(lab_med, Rg, thD, thP, mu_g):
        sD, sP, _o = metrics(lab_med, Rg, ctx, False)
        if not (sD > thD or sP > thP):
            return 1
        if not pre_ref:
            pre_ref.append((median_view(rv, False), noise_model(rv, ctx, False)[0]))
        Rp, mu_p = pre_ref[0]
        dD, _dP, _o = metrics(lab_med, Rp, ctx, False)
        return 1 if (sD - mu_g) < dict(THRESH)[RELEASE] * (dD - mu_p) else 0

    def int_stretch(si, step):
        out = []
        while si in Is and si in s.rows:
            out.append(si)
            si += step
        return out if step > 0 else out[::-1]

    nojudge = {}
    for i, (a, b) in enumerate(lab_runs):
        if any(si not in s.rows for si in range(a, b + 1)):
            nojudge.setdefault(i, ROW_MISSING)
        if i + 1 < len(lab_runs) and any(si not in s.rows for si in range(b + 1, lab_runs[i + 1][0])):
            nojudge.setdefault(i, ROW_MISSING)
            nojudge.setdefault(i + 1, ROW_MISSING)
        if any(si in Us for si in range(a, b + 1)):
            nojudge.setdefault(i, LABELLED_UNPAIRED)

    for i, (a, b) in enumerate(lab_runs):
        if i in nojudge:
            why_unj[(a, b)] = nojudge[i]
            continue
        lo_w = (lab_runs[i - 1][1] + a) // 2 + 1 if i > 0 else span_lo
        hi_w = (b + lab_runs[i + 1][0]) // 2 if i + 1 < len(lab_runs) else span_hi
        pre_int = int_stretch(a - 1, -1)
        if len(pre_int) >= 2:
            ron = pre_int[-POST_REF_N:]
            if i > 0 and len(ron) >= 3 and ron[0] == lab_runs[i - 1][1] + 1:
                ron = ron[1:]
        elif i == 0:
            ron = refc
        else:
            ron = [si for si in range(lab_runs[i - 1][1] + 1, a) if clean(si)]
            if len(ron) < 2:
                ron = refc
        nxt = lab_runs[i + 1][0] if i + 1 < len(lab_runs) else span_hi + 1
        b_next = b + 2 if (b + 1) in Ps else b + 1
        post_int = [si for si in int_stretch(b_next, 1) if si < nxt]
        if len(post_int) >= POST_REF_N + 2:
            roff = post_int[-POST_REF_N:]
        elif post_int:
            roff = (post_int + [si for si in range(post_int[-1] + 1, nxt) if clean(si)])[:POST_REF_N]
        else:
            roff = [si for si in range(b + 1, nxt) if clean(si)][:POST_REF_N]
        on_views = [V(x) for x in ron]
        off_views = [V(x) for x in roff] if len(roff) >= 2 else []
        covered = set()
        if s.brk(a - 1) == "frame png missing":
            covered |= {a - 1, a}
        if s.brk(b) == "frame png missing":
            covered |= {b, b + 1}
        if b_next > b + 1 and s.brk(b + 1) == "frame png missing":
            covered |= {b + 1, b + 2}
        unread = [si for si in range(a - 1, b_next + 1)
                  if si in s.rows and si not in Ns and si not in Us and si not in Ps and si not in covered
                  and V(si) is None]
        off_bad = [x for x, v in zip(roff, off_views) if v is None and x not in covered]
        bad = s.invalid
        if any(x is None for x in on_views):
            why_unj[(a, b)] = "onset reference " + ("invalid" if bad.intersection(ron) else "unreadable")
            continue
        if off_bad:
            why_unj[(a, b)] = "end reference " + ("invalid" if bad.intersection(off_bad) else "unreadable")
            continue
        if unread:
            why_unj[(a, b)] = "frame " + ("invalid" if bad.intersection(unread) else "unreadable")
            continue
        if (b + 1) in Ns:
            cens[("e", b)] = NANITE_REASON
        if (b + 1) in Us:
            cens[("e", b)] = UNPAIRED_REASON
        if (a - 1) in Us:
            cens[("s", a)] = UNPAIRED_REASON
        Ron = median_view(on_views)
        mu_on, thD_on, thP_on, thOB_on = noise_model(on_views, ctx)
        lab_med = None
        if len(pre_int) >= 2 or post_int:
            lab_views = [V(x) for x in range(a, b + 1)]
            lab_med = median_view(lab_views, False) if not any(x is None for x in lab_views) else None
        int_shows = 0
        if len(pre_int) >= 2 and lab_med is not None:
            int_shows += shows_effect(lab_med, Ron, thD_on, thP_on, mu_on)
        off_ok = len(off_views) >= 2 and not any(x is None for x in off_views)
        midr = (a + b) / 2.0
        ser_on = {}
        off_all = {}
        for si in range(lo_w, hi_w + 1):
            if si not in s.rows or si in Ns or si in Us or si in Ps:
                continue
            v = V(si)
            if v is None:
                continue
            D, P, OB = metrics(v, Ron, ctx)
            ser_on[si] = (D, P)
            obflag[si] = OB > thOB_on
        if b_next > b + 1 and b_next not in ser_on:
            cens.setdefault(("e", b), PIE_REASON)
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
            use_si = roff[k:]
            while len(use) >= 3:
                head = use[:-1]
                _mh, thDh, thPh, _th = noise_model(head, ctx, False)
                dl, pl, _o = metrics(use[-1], median_view(head, False), ctx, False)
                if dl <= thDh and pl <= thPh:
                    break
                use = head
                use_si = use_si[:-1]
            roff = roff[:k] + use_si
            Roff = median_view(use, False)
            mu_off, thD_off, thP_off, _t = noise_model(use, ctx, False)
            if post_int and lab_med is not None:
                int_shows += shows_effect(lab_med, Roff, thD_off, thP_off, mu_off)
            for si in ser_on:
                if si >= a:
                    D, P, _o = metrics(V(si), Roff, ctx, False)
                    off_all[si] = (D, P)
            late = [si for si in range(roff[-1] + 1, hi_w + 1) if si in ser_on and clean(si)]
            dD = max([v[0] for v in off_all.values()] or [0.0])
            dP = max([v[1] for v in off_all.values()] or [0.0])
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
                if si >= a:
                    off_all[si] = ser_on[si]
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
        off_items = [(si, off_all[si]) for si in off_all if si > midr]
        dD_on = max([x[1][0] for x in on_items] or [0.0])
        dD_off = max([v[0] for v in off_all.values()] or [dD_on])
        for si, (D, P) in on_items:
            series[si] = dict(D=D, P=P, frac=(D - mu_on) / max(dD_on - mu_on, 1e-6), strict=bool(D > thD_on or P > thP_on))
            sig_by_si[si] = (thD_on - mu_on) / K_SIG
        for si, (D, P) in off_items:
            series[si] = dict(D=D, P=P, frac=(D - mu_off) / max(dD_off - mu_off, 1e-6), strict=bool(D > thD_off or P > thP_off))
            sig_by_si[si] = (thD_off - mu_off) / K_SIG
        run_info.append(dict(a=a, b=b, lo=lo_w, hi=hi_w, thrD_on=thD_on, depth_on=dD_on, fallback=not settled,
                             ron=ron, roff=roff, k=k, depth_off=dD_off, mu_on=mu_on, mu_off=mu_off, thD_off=thD_off,
                             int_start=bool(pre_int), int_end=bool(post_int), int_shows=int_shows))
    judged = set((ri["a"], ri["b"]) for ri in run_info)
    missed = [(a, b) for a, b in lab_runs if (a, b) not in judged]
    res["unjudged"] = [dict(a=a, b=b, why=why_unj.get((a, b), "not judged")) for a, b in missed]
    if not series:
        res["status"] = "UNJUDGED" if missed else "NO-FRAMES"
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
    shows = sum(ri["int_shows"] for ri in run_info)
    per = {}
    for name, t in THRESH:
        vis = set(si for si, v in series.items() if v["strict"] and v["frac"] >= t)
        tg = transition_gate(L, lab_runs, vis, [si for si in span if si in series], reasons, drop, sig, taa)
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
        unres = set()
        for e, ri in zip(edges, run_info):
            for si in tg["unl"]:
                if (e["ue"] and e["b"] < si <= ri["hi"]) or (e["us"] and ri["lo"] <= si < e["a"]):
                    unres.add(si)
        tg_u = dict(tg)
        tg_u["unl_x"] = [si for si in tg["unl_x"] if si not in unres]
        fails, amb = split_censored(tg_u, L, on_c, off_c)
        raw_fails = []
        if [si for si in tg["unl"] if si not in unres and not ((si > max(L) and off_c) or (si < min(L) and on_c))]:
            raw_fails.append("unlabelled visible")
        mid_all = (min(L) + max(L)) / 2.0
        if [si for si in tg["lnv"] if not ((si > mid_all and off_c) or (si <= mid_all and on_c))]:
            raw_fails.append("labelled not visible")
        if shows:
            fails.append("interrupted frames show the effect")
            raw_fails.append("interrupted frames show the effect")
        cen_any = any(e["cs"] or e["ce"] or e["us"] or e["ue"] for e in edges)
        v_ta = event_verdict(res["measurable"], res["confounded"], fails, missed, cen_any)
        v_raw = event_verdict(res["measurable"], res["confounded"], raw_fails, missed, cen_any)
        per[name] = dict(verdict=v_ta, verdict_raw=v_raw, edges=edges, fails=fails, stats=dict(tg["stats"]),
                         excused=len(tg["exc_unl"]) + len(tg["exc_lnv"]), strict00=tg["strict00"])
    res["per"] = per
    res["status"] = "OK"
    res["fallback_runs"] = fallback_runs
    if DEBUG:
        res["dbg"] = dict(series={si: (round(v["D"], 3), round(v["frac"], 3), v["strict"]) for si, v in sorted(series.items())},
                          runs=run_info, refc=refc, span=span)
    res["runs"] = len(run_info)
    res["int_ends"] = sum(1 for ri in run_info if ri["int_end"])
    res["int_shows"] = shows
    res["nanite_ends"] = sum(1 for _a, b in lab_runs if (b + 1) in Ns)
    res["unpaired_ends"] = sum((1 if (ri["a"] - 1) in Us else 0) + (1 if (ri["b"] + 1) in Us else 0) for ri in run_info)
    res["pie_ends"] = sum(1 for ri in run_info if (ri["b"] + 1) in Ps)
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
            if si in Ls or si in Ps:
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
        if si in s.unpaired:
            continue
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
    Us = s.unpaired
    Ns = set(si for si, rs in reasons.items() if NANITE_REASON in (rs or ()) and si not in set(Lw) and si not in Us)
    X = Ns | Us
    hi_cap = s.sis[-1]
    span_hi = min(L1 + SPAN_AFTER, hi_cap, (next_first - PRE_W - 1) if next_first is not None else hi_cap)
    span = [si for si in range(L0 - PRE_W, span_hi + 1) if si in s.rows and si in rho and si not in X]
    if len(span) < PRE_W + len(Lw) + SUFFIX + 2:
        return dict(status="SHORT-SPAN")
    unread = [si for si in range(L0 - 1, L1 + 2) if si in s.rows and si not in rho and si not in X]
    norow = [si for si in range(L0, L1 + 1) if si not in s.rows]
    labu = [si for si in Lw if si in Us]
    why = None
    if norow:
        why = ROW_MISSING
    elif labu:
        why = LABELLED_UNPAIRED
    elif unread:
        why = "frame " + ("invalid" if s.invalid.intersection(unread) else "unreadable")
    unread = sorted(set(unread) | set(norow) | set(labu))
    u_in = any(si in Us for si in range(L0 + 1, L1))
    prew = [si for si in range(L0 - PRE_N, L0) if si in rho and si not in X]
    if len(prew) < 4:
        return dict(status="NO-PRE-LEVEL")
    pre = median([rho[x] for x in prew])
    postw = span[-POST_N:] if span_hi - L1 >= 20 else []
    post = median([rho[x] for x in postw]) if len(postw) >= 4 else None
    first_row = min(e["rows"]) if e["rows"] else L0
    own = set(range(min(L0, first_row) - 1, L1 + SPAN_AFTER // 2))
    ex = excl | own | X
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
        if (L1 + 1) in Ns:
            offc = offc or NANITE_REASON
        if (L1 + 1) in Us:
            offc = offc or UNPAIRED_REASON
        if (L0 - 1) in Us:
            onc = onc or UNPAIRED_REASON
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
        cens = bool(onc or offc or u_in)
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
                post1=post1, warm=L0 < SETTLE_SKIP, unread=unread, why=why, nanite_ends=1 if (L1 + 1) in Ns else 0,
                unpaired_ends=(1 if (L1 + 1) in Us else 0) + (1 if (L0 - 1) in Us else 0))


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
        row = dict(type=typ, ord=ev["ord"], rule=rule, disagree=ev["disagree"], ce=None, unknown=ev["unknown"],
                   int_frames=len(ev["I"]), int_gap=sum(1 for si in ev["I"] if ev["L"] and ev["L"][0] < si < ev["L"][-1]),
                   nanite_frames=len(ev["N"]), unpaired_frames=len(ev["U"]), pie_frames=len(ev["P"]),
                   pie_bad=list(ev["pie_bad"]))
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
                elif m.get("unread") and not (m.get("measurable") and m["per"][RELEASE]["verdict"] == "FAIL"):
                    row["status"] = "UNJUDGED"
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
        if ev["pie_bad"]:
            pie_guard_fail(row, ev["pie_bad"])
        out.append(row)
    return s, rule, taa, out


def pie_guard_fail(row, why):
    per = row.get("per")
    if per is None:
        per = {n: dict(verdict="FAIL", verdict_raw="FAIL", edges=[], fails=[], stats={}, excused=0, strict00=False)
               for n, _t in THRESH}
        row["per"] = per
        row["status"] = "OK"
    for p in per.values():
        p["fails"] = list(p["fails"]) + [w for w in why if w not in p["fails"]]
        p["verdict"] = "FAIL"
        p["verdict_raw"] = "FAIL"


def _d_status(dres):
    st = dres.get("status")
    if st != "OK":
        return dict(status=st)
    v = dres["per"][RELEASE]["verdict"]
    if v in ("NOT-MEASURABLE", "CONFOUNDED", "UNJUDGED"):
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
                rs[si] = (UNPAIRED_REASON,) if si in s.unpaired else ("temporal_aa",)
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
        self.unknown = 0
        self.int_events = 0
        self.int_judged = 0
        self.int_frames = 0
        self.int_gap = 0
        self.int_ends = 0
        self.int_shows = 0
        self.nan_events = 0
        self.nan_frames = 0
        self.nan_ends = 0
        self.un_events = 0
        self.un_frames = 0
        self.un_ends = 0
        self.pie_events = 0
        self.pie_frames = 0
        self.pie_ends = 0
        self.pie_bad_events = 0
        self.pie_bad = collections.Counter()
        self.unj_runs = 0
        self.unj_runs_fail = 0


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
        a.unknown += r.get("unknown") or 0
        if r.get("int_frames"):
            a.int_events += 1
            a.int_frames += r["int_frames"]
            a.int_gap += r.get("int_gap") or 0
        if r.get("nanite_frames"):
            a.nan_events += 1
            a.nan_frames += r["nanite_frames"]
        if r.get("unpaired_frames"):
            a.un_events += 1
            a.un_frames += r["unpaired_frames"]
        if r.get("pie_frames"):
            a.pie_events += 1
            a.pie_frames += r["pie_frames"]
        if r.get("pie_bad"):
            a.pie_bad_events += 1
            a.pie_bad.update(r["pie_bad"])
        st = r.get("status")
        if st == "NOT-JUDGEABLE":
            a.status["not-judgeable"] += 1
            continue
        if r.get("path") == "m52-fallback-d" and st == "OK":
            a.fallback_events += 1
        d = r.get("d") or {}
        if st in ("OK", "UNJUDGED"):
            n_unj = (1 if r["m52"].get("unread") else 0) if r.get("path") == "m52" else len(d.get("unjudged") or [])
            a.unj_runs += n_unj
            if st == "OK":
                a.unj_runs_fail += n_unj
        if d.get("status") == "OK" and d.get("measurable") and st not in ("WARMUP", "CAMERA-MOVED", "UNJUDGED"):
            if d.get("wrong_obj"):
                a.wrong += 1
                if d.get("wrong_obj_clean"):
                    a.wrong_clean += 1
            a.mm += d.get("mask_missing") or 0
            a.mx += d.get("mask_extra") or 0
        if st != "OK":
            a.status[{"WARMUP": "warm-up", "CAMERA-MOVED": "camera moved", "NOT-MEASURABLE": "not measurable",
                      "CONFOUNDED": "confounded", "CONFOUNDED-REFERENCE": "confounded reference",
                      "UNJUDGED": "unjudged"}.get(st, "not judged (no reference or frames)")] += 1
            continue
        a.status["judged"] += 1
        if r.get("int_frames"):
            a.int_judged += 1
            a.int_ends += d.get("int_ends") or 0
            a.int_shows += d.get("int_shows") or 0
        if r.get("nanite_frames"):
            a.nan_ends += (r["m52"] if r.get("path") == "m52" else d).get("nanite_ends") or 0
        a.un_ends += (r["m52"] if r.get("path") == "m52" else d).get("unpaired_ends") or 0
        a.pie_ends += d.get("pie_ends") or 0
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
    w("unpaired frames (written on the sync path, picture not paired with its label; left out of every check) %d in "
      "%d session(s)" % (si.get("unpaired", 0), si.get("unpaired_sessions", 0)))
    aggs = aggregate(rows)
    pie_bad = collections.Counter()
    for a in aggs.values():
        pie_bad.update(a.pie_bad)
    n_pie_out = si["refused"].get(PIE_OUTSIDE, 0)
    n_pie_bad = sum(a.pie_bad_events for a in aggs.values())
    w("pie_end_settle (Play-In-Editor capture; the first frame after a labelled run is left out of its event, never a "
      "reference, and the run end is judged on the next frame): PIE sessions %d | frames dropped %d (%s) | judged run "
      "ends with a dropped frame %d" % (
          si.get("pie", 0), sum(a.pie_frames for a in aggs.values()),
          ", ".join("%s %d" % (t, a.pie_frames) for t, a in aggs.items() if a.pie_frames) or "none",
          sum(a.pie_ends for a in aggs.values())))
    if n_pie_out or n_pie_bad:
        w("*** PIE_END_SETTLE FLAG GUARD FAILED: %d session(s) refused (%s) | %d event(s) failed (%s); the flag "
          "excuses nothing there ***" % (n_pie_out, PIE_OUTSIDE, n_pie_bad,
                                         ", ".join("%s %d" % (k, v) for k, v in sorted(pie_bad.items())) or "none"))
    tot_runs = sum(a.runs for a in aggs.values())
    tot_fb = sum(a.fb for a in aggs.values())
    tot_52fb = sum(a.fallback_events for a in aggs.values())
    w("thresholds: edge-local references (084-06) on %d of %d judged run ends; %d judged on the onset reference "
      "(086-01 constants; no clean frames after the run)" % (tot_runs - tot_fb, tot_runs, tot_fb))
    w("stuck_low_mip: sharpness basis own-detrend, edge-local noise (no null session); %d event(s) judged by the "
      "086-01 path because the sharpness path lacked frames" % tot_52fb)
    w("reference amendment R1 (084-09): %s" % ("on - an event with fewer than %d clean reference frames reads confounded reference" % REF_MIN if REF_AMENDMENT else "off"))
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
                                                                  "confounded", "confounded reference", "unjudged",
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
        w("wrong-object: %d event(s) tied to the label | upper bound %d, of which %d also change on unlabelled frames" % (
            a.wrong - a.wrong_clean, a.wrong, a.wrong_clean))
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
        w("interrupted (effect removed mid-event): events %d, judged %d | run ends at an interruption %d | frames %d, "
          "between two labelled runs %d | interrupted pictures still showing the effect %d" % (
              a.int_events, a.int_judged, a.int_ends, a.int_frames, a.int_gap, a.int_shows))
        w("nanite (object drew Nanite mid-event, frames left out): events %d | frames %d | judged run ends censored there %d" % (
            a.nan_events, a.nan_frames, a.nan_ends))
        w("unpaired (frame written on the sync path, its picture is the previous frame; left out): events %d | frames %d"
          " | judged run edges censored there %d" % (a.un_events, a.un_frames, a.un_ends))
        w("pie_end_settle (first frame after a labelled run left out; the run end judged on the next frame): events %d "
          "| frames dropped %d | judged run ends with a dropped frame %d | flag guard failures %d" % (
              a.pie_events, a.pie_frames, a.pie_ends, a.pie_bad_events))
        w("unjudged (a labels row or an image needed to judge a run is missing, unreadable or invalid): events %d | "
          "runs %d, of them inside failing events %d" % (a.status.get("unjudged", 0), a.unj_runs, a.unj_runs_fail))
        w("transition reasons unknown to this kit: %d frame(s)" % a.unknown)
        w("m55 onset witness: %s" % _counter_line(a.ce_w, ("onset-on-first", "label-early", "no-change", "partial-first",
                                                             "unmeasured")))
        w("")
    w("READ BACK (release reading: transition-aware, 50 %)")
    for t, a in aggs.items():
        if t in NOT_JUDGEABLE:
            w("  %-18s events %d, not judgeable by this kit" % (t, a.events))
            continue
        if not a.status.get("judged"):
            w("  %-18s events %d, judged 0 | nanite %d | unjudged %d | unpaired %d" % (
                t, a.events, a.nan_events, a.status.get("unjudged", 0), a.un_events))
            continue
        w("  %-18s judged %d | start %s | end %s | wrong-object %d (upper bound %d) | censored %d | fail %d | interrupted %d"
          " | nanite %d | unjudged %d | unpaired %d" % (
              t, a.status.get("judged", 0), hist(a.S[RELEASE]["ta"]), hist(a.E[RELEASE]["ta"]), a.wrong - a.wrong_clean,
              a.wrong, a.cs + a.ce + a.us + a.ue, a.release.get("FAIL", 0), a.int_judged, a.nan_events,
              a.status.get("unjudged", 0), a.un_events))
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


def sync_only(path):
    n = 0
    try:
        with open(path, "r", encoding="utf-8-sig") as fh:
            for line in fh:
                line = line.strip()
                if not line:
                    continue
                try:
                    r = json.loads(line)
                except ValueError:
                    continue
                if not isinstance(r, dict) or not isinstance(r.get("session_index"), int):
                    continue
                if not row_unpaired(r):
                    return False
                n += 1
    except OSError:
        return False
    return n > 0


def refuse_reason(d):
    if not os.path.isfile(os.path.join(d, "labels.jsonl")):
        return "no labels"
    anno = jload(os.path.join(d, "annotation.json"), None)
    if not isinstance(anno, dict):
        return "unreadable annotation"
    sch = anno.get("label_schema")
    if sch is None or sch < 2:
        return "label schema 1"
    if sync_only(os.path.join(d, "labels.jsonl")):
        return SYNC_ONLY
    if pie_outside(d):
        return PIE_OUTSIDE
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
                taa=collections.Counter(), nomask=0, unpaired=0, unpaired_sessions=0, pie=0)
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
        if s.unpaired:
            info["unpaired"] += len(s.unpaired)
            info["unpaired_sessions"] += 1
        if s.pie_active:
            info["pie"] += 1
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
        elif kind == "host":
            c = [40 + drift, 160, 40]
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
    kind = "host" if state.get("host") else spec["kind"]
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
        if ev.get("annotated", True):
            anomalies.append({"anomaly_type": typ, "anomaly_subtype": typ, "injected_frames": {"frame_indices": sorted(L)},
                              "affected_frames": {"frame_indices": sorted(L)}, "manifested": True,
                              "affected_objects": {"nodes": [{"name": "T%d" % k}]}})
    gap_at = spec.get("gap_before")
    unp = spec.get("unpaired", {})
    written = {}
    obj_mask = [bytes(ST_MV if (ST_OBJ[0] <= x < ST_OBJ[2] and ST_OBJ[1] <= y < ST_OBJ[3]) else 0 for x in range(W))
                for y in range(H)]
    for si in range(n):
        state = {"f": 0.0}
        for k, ev in enumerate(evs):
            pf = ev.get("pixels", {})
            if si in pf:
                state["f"] = pf[si]
            if si in ev.get("host", ()):
                state["host"] = True
        if spec.get("drift_from") is not None and si >= spec["drift_from"]:
            state["drift"] = (si - spec["drift_from"]) * spec.get("drift_step", 3)
        if spec.get("wrong") and any(si in ev["L"] for ev in evs):
            state["wrong"] = True
        rows = _st_frame(si, spec, state)
        if unp.get(si) == "prev" and (si - 1) in written:
            rows = written[si - 1]
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
                ent["labelled"] = si in L and si not in unp
            fl = ev.get("flags", {}).get(si)
            if si in unp:
                ent["transition"] = 1
                ent["transition_reason"] = [UNPAIRED_REASON] + [q for q in (fl or ()) if q != UNPAIRED_REASON]
            elif fl is not None:
                ent["transition"] = 1
                if fl:
                    ent["transition_reason"] = list(fl)
            if typ == M52:
                rec = ev.get("record", {}).get(si, "held" if si in L else "baseline")
                ent["stuck_mip.held"] = si in L
                ent["stuck_mip.render_state"] = "held" if si in L else "none"
                ent["stuck_mip.textures"] = [{"name": "T_N", "forced_mips": 7, "baseline_mips": 11}]
                ent["stuck_mip.render_textures"] = [{"name": "T_N", "baseline_mips": 11, "held_level_mips": 7, "level": rec}]
            transition_only = fl is not None and si not in L and (si > max(L) or "effect_interrupted" in fl)
            if not transition_only:
                present = True
            entries.append(ent)
            if si in L and si not in ev.get("mask_missing", ()) and si not in unp:
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
        if si in unp:
            row.pop("mask_file")
            row.update({"capture_unpaired": True, "transition_reason": [UNPAIRED_REASON], "anomaly_present": False,
                        "visible_positive": False})
            if entries:
                row["transition_present"] = True
        labels.append(row)
    omit_rows = set(spec.get("omit_rows", ()))
    labels = [r for r in labels if r["session_index"] not in omit_rows]
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
        f1 = any("effect_interrupted" in (fl or ()) for ev in evs for fl in ev.get("flags", {}).values())
        summ["label_labelled_rule"] = ("annotation_membership_per_policy_v2_effect_installed" if f1
                                       else "annotation_membership_per_policy_v1")
    if unp:
        summ["capture_unpaired_frames"] = len(unp)
        summ["label_transition_capture_unpaired_entries"] = sum(
            1 for r in labels if r.get("capture_unpaired") for _x in r["anomalies"])
    if spec.get("pie") is not None:
        pie_ents = [x for r in labels for x in r["anomalies"] if _has_pie(x.get("transition_reason"))]
        summ["pie_end_settle_active"] = bool(spec["pie"])
        summ["pie_end_settle_frames"] = sum(
            1 for r in labels if any(_has_pie(x.get("transition_reason")) for x in r["anomalies"]))
        summ["label_transition_pie_end_settle_entries"] = len(pie_ents)
    with open(os.path.join(d, "run_summary.json"), "w", encoding="utf-8") as fh:
        json.dump(summ, fh)
    for si in spec.get("omit", ()):
        os.remove(os.path.join(d, "Actual_Frames", "frame_%05d.png" % si))
    for si in spec.get("zero", ()):
        open(os.path.join(d, "Actual_Frames", "frame_%05d.png" % si), "wb").close()
    for si, how in spec.get("damage", {}).items():
        st_damage(os.path.join(d, "Actual_Frames", "frame_%05d.png" % si), how)
    return d, written


def st_damage(path, how):
    with open(path, "rb") as fh:
        data = fh.read()
    pos = 8
    idat = None
    while pos < len(data):
        length, ctype = struct.unpack_from(">I4s", data, pos)
        if ctype == b"IDAT" and idat is None:
            idat = (pos, length)
        pos += 12 + length
    ip, il = idat
    if how == "mid":
        data = data[:ip + 8 + il // 2]
    elif how == "tail":
        data = data[:ip + 8 + il - 4]
    elif how == "badcrc":
        k = ip + 8 + il
        data = data[:k] + bytes([data[k] ^ 0x5a]) + data[k + 1:]
    elif how == "badihdr":
        data = data[:29] + bytes([data[29] ^ 0x5a]) + data[30:]
    elif how == "noiend":
        data = data[:ip + 12 + il]
    elif how == "after":
        data = data + b"\x00\x00\x00\x00"
    else:
        raise ValueError(how)
    with open(path, "wb") as fh:
        fh.write(data)


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
    late_change = _full(L)
    late_change.update({60: 1.0, 61: 1.0, 62: 1.0, 63: 1.0})
    tex("later_change_at_window_end", late_change)
    stray = _full(L)
    stray[55] = 1.0
    tex("stray_visible_frame", stray)
    cases.append(("unannotated_burst_after", {"type": "corrupted_texture", "kind": "tex", "n": 72, "events": [
        {"L": L, "pixels": _full(L)},
        {"L": [58, 59, 60], "pixels": _full([58, 59, 60]), "annotated": False}]}))
    tex("old_rule_late_1", _full(range(40, 48)), labels=list(range(41, 48)), rule="old")
    smear = _full(L)
    smear.update({48: 0.7, 49: 0.35, 50: 0.1})
    flags = {si: ("temporal_aa",) for si in range(48, 56)}
    flags.update({si: ("temporal_aa",) for si in (40, 41, 42)})
    tex("taa_smear_flagged", smear, taa=True, ev={"flags": flags})
    wrongflag = {39: ("temporal_aa",), 48: ("temporal_aa",)}
    tex("late_1_flag_covering", _full(range(39, 47)), labels=list(range(40, 49)), taa=True, ev={"flags": wrongflag})
    cut = ("effect_interrupted",)
    tex("f1_interrupted_end", _full(L), ev={"flags": {si: cut for si in range(48, 60)}})
    L2 = list(range(40, 45)) + list(range(48, 55))
    tex("f1_host_reinstall", _full(L2), labels=L2, ev={"flags": {si: cut for si in (45, 46, 47)}, "host": (45, 46, 47)})
    tex("f1_host_to_capture_end", _full(L), ev={"flags": {si: cut for si in range(48, 72)}, "host": tuple(range(48, 72))})
    tex("f1_label_past_interruption_4", _full(L), labels=list(range(40, 52)), ev={"flags": {si: cut for si in range(52, 60)}})
    tex("f1_label_stops_early_2", _full(range(40, 50)), taa=True, ev={"flags": {si: cut for si in range(48, 60)}})
    tex("f1_gap_shows_effect", _full(range(40, 55)), labels=L2, ev={"flags": {si: cut for si in (45, 46, 47)}})
    smear48 = _full(L)
    smear48[48] = 0.7
    tex("interrupt_plus_aa", smear48, taa=True,
        ev={"flags": {si: (cut + ("temporal_aa",) if si == 48 else cut) for si in range(48, 60)}})
    R2 = L + list(range(56, 64))
    rflags = {si: cut for si in range(48, 56)}
    wrong2 = _full(R2)
    wrong2.pop(56)
    tex("reinstall_complete_control", _full(R2), labels=R2, ev={"flags": rflags})
    tex("reinstall_wrong_control", wrong2, labels=R2, ev={"flags": rflags})
    tex("reinstall_missing_reference", _full(R2), labels=R2, ev={"flags": rflags}, omit=(54,))
    tex("reinstall_wrong_missing_reference", wrong2, labels=R2, ev={"flags": rflags}, omit=(54,))
    tex("edge_frame_unreadable", _full(range(40, 48)), labels=list(range(40, 47)), zero=(47,))
    tex("onset_frame_unreadable", _full(range(39, 48)), zero=(39,))
    tex("edge_frame_missing_censored", _full(range(40, 48)), labels=list(range(40, 47)), omit=(47,))
    hole = _full(L)
    hole.pop(44)
    tex("interior_frame_missing", hole, omit=(44,))
    tex("nanite_end", _full(range(40, 50)), ev={"flags": {si: ("nanite_unmaskable",) for si in range(48, 56)}})
    R3 = L + list(range(60, 68))
    tex("fail_beside_unjudged_run", _full(list(range(40, 49)) + list(range(60, 68))), labels=R3, omit=(57,))
    rm = _full(L)
    rm.pop(44)
    tex("labels_row_missing_in_run", rm, omit_rows=(44,))
    tex("labels_rows_missing_between_runs", _full(range(40, 56)), labels=list(range(40, 44)) + list(range(48, 56)),
        omit_rows=(44, 45, 46, 47))
    tex("frame_cut_mid", _full(L), damage={44: "mid"})
    tex("frame_cut_tail", _full(L), damage={44: "tail"})
    tex("frame_bad_crc", _full(range(40, 48)), labels=list(range(40, 47)), damage={47: "badcrc"})
    tex("unpaired_end_prev", _full(L), unpaired={48: "prev"})
    tex("unpaired_end_clean", _full(L), unpaired={48: "own"})
    tex("unpaired_before_onset", _full(L), unpaired={39: "prev"})
    tex("unpaired_after_late_1", _full(range(40, 47)), labels=L, unpaired={48: "prev"})
    ghost48 = _full(L)
    ghost48[48] = 0.7
    tex("unpaired_aa_no_excuse", ghost48, taa=True, unpaired={48: "own"},
        ev={"flags": {48: (UNPAIRED_REASON, "temporal_aa")}})
    tex("unpaired_inside_hole", _full(L), labels=[40, 41, 42, 43, 45, 46, 47], unpaired={44: "prev"})
    tex("unpaired_listed_in_annotation", _full(L), unpaired={44: "prev"})
    tex("all_unpaired", _full(L), unpaired={si: "prev" for si in range(72)}, ev={"annotated": False})

    def pie(name, pixels, flags=(48,), active=True, typ="missing_texture", kind="tex"):
        ev = {"L": L, "pixels": pixels, "flags": {si: (PIE_REASON,) for si in flags}}
        cases.append((name, {"type": typ, "kind": kind, "n": 72, "events": [ev], "pie": active}))

    pie("pie_settle_one_late", _full(range(40, 49)))
    pie("pie_settle_late_2", _full(range(40, 50)))
    pie("pie_settle_not_pie", _full(range(40, 49)), active=False)
    pie("pie_settle_unflagged_late_1", _full(range(40, 49)), flags=())
    pie("pie_settle_on_clean_frame", _full(L))
    pie("pie_settle_on_blinking", _full(L), typ="blink", kind="hide")
    pie("pie_settle_two_flags", _full(range(40, 50)), flags=(48, 49))
    pie("pie_settle_labelled", _full(L), flags=(47,))
    ghost = {si: 1.0 for si in L}
    ghost[48] = 0.08
    cases.append(("ghost_8pct", {"type": "blink", "kind": "hide", "n": 72, "events": [{"L": L, "pixels": ghost}]}))
    g2 = {40: 1.0, 41: 1.0, 42: 1.0, 46: 1.0, 47: 0.15}
    cases.append(("blink_single_run_residual", {"type": "blink", "kind": "hide", "n": 72,
                                                "events": [{"L": [40, 41, 42, 46], "pixels": g2}]}))
    cases.append(("cc", {"type": "camera_clipping", "kind": "tex", "n": 60, "events": [{"L": list(range(40, 44)),
                                                                                         "pixels": {}}]}))
    L52 = list(range(60, 70))

    def m52(name, pixels, flags=None, record=None, taa=False, labels=None, fire_pre=1, zero=(), **kw):
        ev = {"L": labels or L52, "pixels": pixels, "flags": flags or {}, "record": record or {}, "fire_pre": fire_pre}
        spec = {"type": M52, "kind": "blur", "n": 190, "events": [ev], "taa": taa, "zero": zero}
        spec.update(kw)
        cases.append((name, spec))

    m52("m52_exact", _full(L52))
    m52("m52_label_late_3", _full(L52), labels=list(range(63, 73)), fire_pre=4)
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
    m52("m52_onset_frame_unreadable", _full(L52), labels=list(range(61, 70)), fire_pre=2, zero=(60,))
    m52("m52_late_3_unreadable_end", _full(L52), labels=list(range(63, 73)), fire_pre=4, zero=(73,))
    p = _full(L52)
    p[65] = 0.0
    m52("m52_labels_row_missing", p, omit_rows=(65,))
    m52("m52_frame_invalid", _full(L52), damage={65: "tail"})
    m52("m52_unpaired_end", _full(L52), unpaired={70: "prev"})
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
    add("blink_single_run_residual", "one-frame hidden run then a 15 % residual: 50 % end 0, strict end +1",
        lambda r: verdict(r) == "PASS" and r["per"]["t50"]["edges"][1]["end"] == 0
        and r["per"]["strict"]["edges"][1]["end"] == 1)
    add("slow_drift", "slow drift after the event: end censored, not failed",
        lambda r: verdict(r) == "CENSORED" and _edge(r)["ce"])
    add("missing_mask", "one labelled frame without a mask is counted",
        lambda r: r["d"]["mask_missing"] == 1 and verdict(r) == "PASS")
    add("moving_camera", "moving camera is not judged", lambda r: r["status"] == "CAMERA-MOVED")
    add("gap_exact", "gap beside the onset, exact labels: PASS, gap counted",
        lambda r: verdict(r) == "PASS" and _edge(r)["gs"])
    add("gap_late_1", "gap beside the onset, label late by 1: FAIL (a gap never excuses)",
        lambda r: verdict(r) == "FAIL" and _edge(r)["start"] == -1, True)
    add("later_change_at_window_end", "a separate change running into the window end: unresolved, not failed",
        lambda r: verdict(r) == "CENSORED" and _edge(r)["ue"] and not r["per"]["t50"]["fails"])
    add("stray_visible_frame", "an isolated unlabelled visible frame after the event FAILS",
        lambda r: verdict(r) == "FAIL", True)
    add("unannotated_burst_after", "a later burst missing from the annotation (cut or vetoed) ends the window: PASS 0/0",
        lambda r: verdict(r) == "PASS" and _edge(r)["end"] == 0)
    add("wrong_object", "second object changing on labelled frames counted as wrong-object",
        lambda r: r["d"]["wrong_obj"] and not r["d"]["wrong_obj_clean"])
    add("exact", "exact case has no wrong-object", lambda r: not r["d"]["wrong_obj"])
    add("old_rule_late_1", "old-rule session read raw, late by 1 FAILS",
        lambda r: verdict(r) == "FAIL" and raw(r) == "FAIL" and r["rule"] == "old", True)
    add("taa_smear_flagged", "flagged TAA smear: transition-aware PASS end 0, raw end +1",
        lambda r: verdict(r) == "PASS" and _edge(r)["end_ta"] == 0 and _edge(r)["end"] == 1 and raw(r) == "FAIL")
    add("late_1_flag_covering", "label late by 1 with the flag wrongly covering it FAILS at 50 %",
        lambda r: verdict(r) == "FAIL", True)
    def all_edges_zero(r):
        return all(e["start_ta"] == 0 and e["end_ta"] == 0 for e in r["per"]["t50"]["edges"])

    add("f1_interrupted_end", "effect_interrupted after the label, AA off: PASS 0/0, counted, not a TAA flag",
        lambda r: verdict(r) == "PASS" and all_edges_zero(r) and r["int_frames"] == 12 and r["d"]["int_ends"] == 1
        and r["int_gap"] == 0)
    add("f1_host_reinstall", "host material in a 3-frame interruption, re-installed: both runs 0/0, gap 3",
        lambda r: verdict(r) == "PASS" and len(r["per"]["t50"]["edges"]) == 2 and all_edges_zero(r)
        and r["int_gap"] == 3 and r["d"]["int_shows"] == 0)
    add("f1_host_to_capture_end", "host material from the interruption to the capture end: PASS 0/0",
        lambda r: verdict(r) == "PASS" and all_edges_zero(r) and r["d"]["int_ends"] == 1)
    add("f1_label_past_interruption_4", "label runs 4 frames past the interruption FAILS, end -4",
        lambda r: verdict(r) == "FAIL" and _edge(r)["end"] == -4 and _edge(r)["end_ta"] == -4, True)
    add("f1_label_stops_early_2", "effect still visible 2 frames into the interruption, TAA on: FAIL, end +2 (not excused)",
        lambda r: verdict(r) == "FAIL" and _edge(r)["end_ta"] == 2, True)
    add("f1_gap_shows_effect", "interruption gap still showing the effect FAILS",
        lambda r: verdict(r) == "FAIL" and r["d"]["int_shows"] >= 1
        and "interrupted frames show the effect" in r["per"]["t50"]["fails"], True)
    add("interrupt_plus_aa", "AA flag on an interrupted frame excuses nothing: FAIL, end +1",
        lambda r: verdict(r) == "FAIL" and _edge(r)["end"] == 1 and _edge(r)["end_ta"] == 1
        and "unlabelled visible" in r["per"]["t50"]["fails"], True)
    add("reinstall_complete_control", "re-install after an 8-frame interruption, all images: PASS 0/0 both runs",
        lambda r: verdict(r) == "PASS" and len(r["per"]["t50"]["edges"]) == 2 and all_edges_zero(r))
    add("reinstall_wrong_control", "re-install 1 frame early, all images: FAIL, second run start +1",
        lambda r: verdict(r) == "FAIL" and r["per"]["t50"]["edges"][1]["start"] == 1, True)
    add("reinstall_missing_reference", "re-install, one reference image missing: unjudged, both runs named",
        lambda r: r["status"] == "UNJUDGED" and "per" not in r and sorted((u["a"], u["why"]) for u in r["d"]["unjudged"])
        == [(40, "end reference unreadable"), (56, "onset reference unreadable")])
    add("reinstall_wrong_missing_reference", "re-install 1 frame early, one reference image missing: unjudged",
        lambda r: r["status"] == "UNJUDGED" and "per" not in r, True)
    add("edge_frame_unreadable", "unreadable frame after the end, label 1 early: unjudged",
        lambda r: r["status"] == "UNJUDGED" and r["d"]["unjudged"][0]["why"] == "end reference unreadable", True)
    add("onset_frame_unreadable", "unreadable frame before the onset, label 1 late: unjudged",
        lambda r: r["status"] == "UNJUDGED" and r["d"]["unjudged"][0]["why"] == "frame unreadable", True)
    add("edge_frame_missing_censored", "missing frame after the end, label 1 early: censored as before",
        lambda r: verdict(r) == "CENSORED" and _edge(r)["ce"], True)
    add("interior_frame_missing", "missing frame inside a labelled run that hides a gap: unjudged",
        lambda r: r["status"] == "UNJUDGED" and r["d"]["unjudged"][0]["why"] == "frame unreadable", True)
    add("nanite_end", "nanite_unmaskable after the label: not FAIL, end censored, start 0",
        lambda r: verdict(r) == "CENSORED" and _edge(r)["ce"] and _edge(r)["start"] == 0
        and not r["per"]["t50"]["fails"] and r["nanite_frames"] == 8 and r["int_frames"] == 0 and r["unknown"] == 0)
    add("m52_onset_frame_unreadable", "stuck_low_mip, unreadable frame before the onset, label 1 late: unjudged",
        lambda r: r["status"] == "UNJUDGED" and r["path"] == "m52", True)
    add("fail_beside_unjudged_run", "run 1 ends 1 early, run 2 onset reference missing: FAIL, run 2 listed",
        lambda r: verdict(r) == "FAIL" and len(r["per"]["t50"]["edges"]) == 1 and _edge(r)["end"] == 1
        and r["d"]["unjudged"] == [dict(a=60, b=67, why="onset reference unreadable")], True)
    add("m52_late_3_unreadable_end", "stuck_low_mip label 3 late with an unreadable frame after: FAIL, not unjudged",
        lambda r: verdict(r) == "FAIL" and r["path"] == "m52" and r["m52"]["unread"] == [73], True)
    add("labels_row_missing_in_run", "labels row missing inside a labelled run: unjudged, not skipped",
        lambda r: r["status"] == "UNJUDGED" and "per" not in r
        and r["d"]["unjudged"] == [dict(a=40, b=47, why=ROW_MISSING)], True)
    add("labels_rows_missing_between_runs", "labels rows missing in the gap between two runs: both runs unjudged",
        lambda r: r["status"] == "UNJUDGED" and "per" not in r
        and sorted((u["a"], u["why"]) for u in r["d"]["unjudged"]) == [(40, ROW_MISSING), (48, ROW_MISSING)], True)
    add("m52_labels_row_missing", "stuck_low_mip, labels row missing inside the label: unjudged",
        lambda r: r["status"] == "UNJUDGED" and r["path"] == "m52" and r["m52"]["why"] == ROW_MISSING, True)
    add("frame_cut_mid", "PNG cut in the middle of its image data inside a run: invalid, unjudged",
        lambda r: r["status"] == "UNJUDGED" and r["d"]["unjudged"] == [dict(a=40, b=47, why="frame invalid")])
    add("frame_cut_tail", "PNG cut inside its checksum tail (every row still decodes): invalid, unjudged",
        lambda r: r["status"] == "UNJUDGED" and r["d"]["unjudged"] == [dict(a=40, b=47, why="frame invalid")])
    add("frame_bad_crc", "bad chunk CRC on the frame after the end, label 1 early: invalid, unjudged",
        lambda r: r["status"] == "UNJUDGED" and r["d"]["unjudged"][0]["why"] == "end reference invalid", True)
    add("m52_frame_invalid", "stuck_low_mip, PNG cut inside its checksum tail inside the label: unjudged",
        lambda r: r["status"] == "UNJUDGED" and r["path"] == "m52" and r["m52"]["why"] == "frame invalid")

    def unp_end(r):
        e = _edge(r)
        return (verdict(r) == "CENSORED" and e["ce"] and e["cr"] == UNPAIRED_REASON and not r["per"]["t50"]["fails"]
                and r["unpaired_frames"] == 1 and r["unknown"] == 0 and r["d"]["unpaired_ends"] == 1)

    add("unpaired_end_prev", "unpaired frame after the end showing the previous picture: censored, not FAIL", unp_end)
    add("unpaired_end_clean", "unpaired frame after the end with a clean picture: censored, not PASS", unp_end)
    add("unpaired_after_late_1", "label 1 late beside an unpaired frame: censored, not PASS, not FAIL", unp_end, True)
    add("unpaired_before_onset", "unpaired frame before the onset (previous, clean picture): onset censored, not PASS",
        lambda r: verdict(r) == "CENSORED" and _edge(r)["cs"] and not _edge(r)["ce"] and _edge(r)["cr"] == UNPAIRED_REASON
        and not r["per"]["t50"]["fails"] and r["unpaired_frames"] == 1 and r["d"]["unpaired_ends"] == 1)
    add("unpaired_aa_no_excuse", "unpaired frame also flagged temporal_aa: no anti-aliasing excuse, censored",
        lambda r: unp_end(r) and r["per"]["t50"]["excused"] == 0 and _edge(r)["end_ta"] == 0)
    add("unpaired_inside_hole", "unpaired frame between two runs: both edges beside it censored, not FAIL",
        lambda r: verdict(r) == "CENSORED" and len(r["per"]["t50"]["edges"]) == 2 and r["per"]["t50"]["edges"][0]["ce"]
        and r["per"]["t50"]["edges"][1]["cs"] and r["per"]["t50"]["edges"][1]["cr"] == UNPAIRED_REASON
        and not r["per"]["t50"]["fails"] and r["d"]["unpaired_ends"] == 2)
    add("unpaired_listed_in_annotation", "annotation listing an unpaired frame: unjudged",
        lambda r: r["status"] == "UNJUDGED" and r["d"]["unjudged"] == [dict(a=40, b=47, why=LABELLED_UNPAIRED)], True)
    add("m52_unpaired_end", "stuck_low_mip, unpaired frame after the label: end censored, not FAIL",
        lambda r: verdict(r) == "CENSORED" and r["path"] == "m52" and _edge(r)["ce"] and not r["per"]["t50"]["fails"]
        and r["m52"]["unpaired_ends"] == 1)

    def pie_fails(r, why):
        return verdict(r) == "FAIL" and raw(r) == "FAIL" and why in r["per"]["t50"]["fails"] and r["pie_bad"]

    def pie_pass(r):
        e = _edge(r)
        return (verdict(r) == "PASS" and raw(r) == "PASS" and e["end"] == 0 and e["end_ta"] == 0 and e["start"] == 0
                and not e["ce"] and r["pie_frames"] == 1 and r["d"]["pie_ends"] == 1 and not r["pie_bad"]
                and r["unknown"] == 0 and r["per"]["t50"]["excused"] == 0)

    add("pie_settle_one_late", "PIE: picture changed 1 frame past the label, that frame pie_end_settle: PASS 0/0",
        pie_pass)
    add("pie_settle_late_2", "PIE: still changed 2 frames past the label, only the first flagged: FAIL, end +2",
        lambda r: verdict(r) == "FAIL" and _edge(r)["end"] == 2 and not _edge(r)["ce"] and not r["pie_bad"]
        and "unlabelled visible" in r["per"]["t50"]["fails"] and r["pie_frames"] == 1, True)
    add("pie_settle_not_pie", "pie_end_settle in a capture that is not PIE: flag ignored, FAIL",
        lambda r: pie_fails(r, PIE_OUTSIDE) and r["pie_frames"] == 0 and _edge(r)["end"] == 1, True)
    add("pie_settle_unflagged_late_1", "PIE: changed 1 frame past the label, no flag: FAIL, end +1",
        lambda r: verdict(r) == "FAIL" and _edge(r)["end"] == 1 and r["pie_frames"] == 0 and not r["pie_bad"], True)
    add("pie_settle_on_clean_frame", "PIE: picture ends with the label, flagged frame clean: PASS 0/0", pie_pass)
    add("pie_settle_on_blinking", "pie_end_settle on blinking (no fire window): FAIL",
        lambda r: pie_fails(r, PIE_TYPE) and r["pie_frames"] == 0, True)
    add("pie_settle_two_flags", "PIE: changed 2 frames past the label, both flagged: FAIL",
        lambda r: pie_fails(r, PIE_PLACE) and r["pie_frames"] == 1, True)
    add("pie_settle_labelled", "pie_end_settle on a labelled frame: FAIL",
        lambda r: pie_fails(r, PIE_LABELLED) and r["pie_frames"] == 0, True)
    add("cc", "camera_clipping is not judgeable", lambda r: r["status"] == "NOT-JUDGEABLE" and "per" not in r)
    add("m52_exact", "stuck_low_mip exact PASS 0/0", lambda r: verdict(r) == "PASS" and _edge(r)["start"] == 0 and _edge(r)["end"] == 0)
    add("m52_label_late_3", "stuck_low_mip label 3 frames late FAILS, start -3",
        lambda r: verdict(r) == "FAIL" and _edge(r)["start"] == -3, True)
    add("m52_partial_confirmed","partial frames above the noise band: transition-aware PASS, raw start +2",
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
        good = os.path.join(root, "good.png")
        src5 = _st_frame(5, {"kind": "tex"}, {"f": 1.0})
        write_png(good, ST_W, ST_H, src5, 2)
        dmg_ok = True
        dmg_n = 0
        for how in ("mid", "tail", "badcrc", "badihdr", "noiend", "after"):
            p = os.path.join(root, "damaged_%s.png" % how)
            shutil.copyfile(good, p)
            st_damage(p, how)
            for dec in decs:
                for mr in (None, 8):
                    try:
                        decode_native(p, dec, mr, None if mr is None else 16)
                        dmg_ok = False
                    except PngError:
                        pass
                    except Exception:
                        dmg_ok = False
                    dmg_n += 1
        for dec in decs:
            w, h, ct, plte, nc, rows = decode_native(good, dec)
            dmg_ok = dmg_ok and rows_to_rgb(rows, ct, plte, nc) == src5
        lines.append("SELFTEST %-66s %s" % ("damaged PNGs (cut, bad CRC, no IEND, bytes after it) refused by every decoder, full and partial (%d decodes)" % dmg_n,
                                          "ok" if dmg_ok else "*** WRONG ***"))
        ok_all = ok_all and dmg_ok
        Lg = list(range(40, 48))
        vis_g = set(range(40, 49))
        span_g = list(range(30, 60))
        drop_g = {si: (1.0 if si in vis_g else 0.0) for si in span_g}
        drop_g[48] = 0.5
        tg_u = transition_gate(Lg, [[40, 47]], vis_g, span_g, {48: (UNPAIRED_REASON, "temporal_aa")}, drop_g, 0.01, True)
        tg_c = transition_gate(Lg, [[40, 47]], vis_g, span_g, {48: ("temporal_aa",)}, drop_g, 0.01, True)
        gate_ok = (48 in tg_u["unl_x"] and 48 not in tg_u["exc_unl"] and 48 in tg_c["exc_unl"]
                   and 48 not in tg_c["unl_x"])
        lines.append("SELFTEST %-66s %s" % ("transition gate: temporal_aa excuses a tail frame, never one also marked capture_unpaired",
                                          "ok" if gate_ok else "*** WRONG ***"))
        ok_all = ok_all and gate_ok
        tg_p = transition_gate(Lg, [[40, 47]], vis_g, span_g, {48: (PIE_REASON, "temporal_aa")}, drop_g, 0.01, True)
        tg_q = transition_gate(Lg, [[40, 47]], vis_g, span_g, {48: (PIE_REASON,)}, drop_g, 0.01, True)
        pgate_ok = (48 in tg_p["unl_x"] and 48 not in tg_p["exc_unl"] and 48 in tg_q["unl_x"]
                    and 48 not in tg_q["exc_unl"] and "unlabelled visible" in tg_q["fails"])
        lines.append("SELFTEST %-66s %s" % ("transition gate: a pie_end_settle frame is never an anti-aliasing excuse",
                                          "ok" if pgate_ok else "*** WRONG ***"))
        ok_all = ok_all and pgate_ok
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
        info_h, rows_h, _el, _fr = run([made["all_unpaired"]], "stdlib")
        ref_ok = (refuse_reason(made["all_unpaired"]) == SYNC_ONLY and refuse_reason(made["unpaired_end_prev"]) is None
                  and info_h["read"] == 0 and info_h["refused"].get(SYNC_ONLY) == 1 and not rows_h)
        lines.append("SELFTEST %-66s %s" % ("a session made only of unpaired (sync-path) frames is refused, a mixed one is read",
                                          "ok" if ref_ok else "*** WRONG ***"))
        ok_all = ok_all and ref_ok
        info_p, rows_p, _el, _fr = run([made["pie_settle_not_pie"]], "stdlib")
        text_p = report(info_p, rows_p, "stdlib", 0.0, 0)
        pref_ok = (refuse_reason(made["pie_settle_not_pie"]) == PIE_OUTSIDE
                   and refuse_reason(made["pie_settle_one_late"]) is None
                   and refuse_reason(made["exact"]) is None
                   and info_p["read"] == 0 and info_p["refused"].get(PIE_OUTSIDE) == 1 and not rows_p
                   and ("*** PIE_END_SETTLE FLAG GUARD FAILED: 1 session(s) refused (%s)" % PIE_OUTSIDE) in text_p)
        lines.append("SELFTEST %-66s %s" % ("pie_end_settle outside a PIE capture refuses the session, loudly; a PIE one is read",
                                          "ok" if pref_ok else "*** WRONG ***"))
        ok_all = ok_all and pref_ok
        info, rows, el, fr = run([made[n] for n, _s in cases], "stdlib")
        text = report(info, rows, "stdlib", el, fr)
        clean = not re.search(r"[\\/]|\.png|\.json|session_|frame_\d|\bT\d\b|st_|label_sync_selftest", text)
        lines.append("SELFTEST %-66s %s" % ("report prints numbers only (no path, folder or object name)", "ok" if clean else "*** WRONG ***"))
        ok_all = ok_all and clean
        ct = [x for x in rows if x["type"] == "corrupted_texture"]
        want_int = sum(1 for x in ct if x.get("status") == "OK" and x.get("int_frames"))
        tl = text.splitlines()
        rb = [l for l in tl if l.strip().startswith("corrupted_texture") and "| interrupted " in l]
        m = (re.search(r"\| interrupted (\d+) \| nanite (\d+) \| unjudged (\d+) \| unpaired (\d+)$", rb[0].rstrip())
             if len(rb) == 1 else None)
        rb_ok = want_int > 0 and m is not None and int(m.group(1)) == want_int
        lines.append("SELFTEST %-66s %s" % ("READ BACK carries the interrupted count (%d judged)" % want_int, "ok" if rb_ok else "*** WRONG ***"))
        ok_all = ok_all and rb_ok
        nan_ok = (m is not None and int(m.group(2)) == 1 and sum(1 for x in ct if x.get("nanite_frames")) == 1)
        lines.append("SELFTEST %-66s %s" % ("READ BACK carries the nanite count (1 event)", "ok" if nan_ok else "*** WRONG ***"))
        ok_all = ok_all and nan_ok
        want_unj = len(("reinstall_missing_reference", "reinstall_wrong_missing_reference", "edge_frame_unreadable",
                        "onset_frame_unreadable", "interior_frame_missing", "labels_row_missing_in_run",
                        "labels_rows_missing_between_runs", "frame_cut_mid", "frame_cut_tail", "frame_bad_crc",
                        "unpaired_listed_in_annotation"))
        hd = tl.index("== corrupted_texture ==") if "== corrupted_texture ==" in tl else -1
        ev_line = tl[hd + 1] if hd >= 0 else ""
        sec = []
        for l in tl[hd + 1:] if hd >= 0 else []:
            if l.startswith("=="):
                break
            sec.append(l)
        unj_line = [l for l in sec if l.startswith("unjudged (")]
        unj_ok = (m is not None and int(m.group(3)) == want_unj and ("| unjudged %d |" % want_unj) in ev_line
                  and sum(1 for x in ct if x.get("status") == "UNJUDGED") == want_unj
                  and len(unj_line) == 1 and unj_line[0].endswith("events %d | runs 15, of them inside failing events 1"
                                                                  % want_unj))
        lines.append("SELFTEST %-66s %s" % ("READ BACK unjudged counts only unjudged events (%d); a failing one is listed" % want_unj,
                                          "ok" if unj_ok else "*** WRONG ***"))
        ok_all = ok_all and unj_ok
        want_un = len(("unpaired_end_prev", "unpaired_end_clean", "unpaired_before_onset", "unpaired_after_late_1",
                       "unpaired_aa_no_excuse", "unpaired_inside_hole", "unpaired_listed_in_annotation"))
        un_line = [l for l in sec if l.startswith("unpaired (")]
        un_ok = (m is not None and int(m.group(4)) == want_un
                 and sum(1 for x in ct if x.get("unpaired_frames")) == want_un
                 and len(un_line) == 1 and un_line[0].endswith("events %d | frames %d | judged run edges censored there 7"
                                                               % (want_un, want_un))
                 and "unpaired frames (written on the sync path" in text and ") 8 in 8 session(s)" in text)
        lines.append("SELFTEST %-66s %s" % ("READ BACK carries the unpaired count (%d events); the refused session is not read" % want_un,
                                          "ok" if un_ok else "*** WRONG ***"))
        ok_all = ok_all and un_ok
        mt_sec = []
        if "== missing_texture ==" in tl:
            for l in tl[tl.index("== missing_texture ==") + 1:]:
                if l.startswith("=="):
                    break
                mt_sec.append(l)
        pie_line = [l for l in mt_sec if l.startswith("pie_end_settle (")]
        pie_ok = (info["pie"] == 7 and "): PIE sessions 7 | frames dropped 4 (missing_texture 4) | judged run ends with a "
                  "dropped frame 4" in text
                  and ("1 session(s) refused (%s) | 3 event(s) failed (" % PIE_OUTSIDE) in text
                  and len(pie_line) == 1 and pie_line[0].endswith(
                      "events 4 | frames dropped 4 | judged run ends with a dropped frame 4 | flag guard failures 2"))
        lines.append("SELFTEST %-66s %s" % ("report carries the pie_end_settle drops (4 frames) and its guard failures",
                                          "ok" if pie_ok else "*** WRONG ***"))
        ok_all = ok_all and pie_ok
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
