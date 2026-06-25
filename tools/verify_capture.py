#!/usr/bin/env python3
"""
verify_capture.py — overlay the m7 capture/labeling bounding boxes onto the captured frames.

The AnomalyInjector capture writes, per frame, an image (frame_<GFrameCounter>.png/.jpg) plus one JSON
line in labels.jsonl. This tool reads that sidecar, draws each anomaly's bbox_px on its image, annotates
it with the anomaly id + target name, and writes an annotated copy to <dir>/annotated/. It also prints a
per-frame summary so you can confirm the temporal label (anomaly_present) and the spatial label (the box).

Usage:
    python verify_capture.py [--dir <captureDir>] [--out <annotatedDir>]

Defaults to the manual single-shot dir:
    D:/IntrusiveAnomalies/StackOBot/Saved/AnomalyCaptures/manual

Requires Pillow:  pip install pillow
"""

import argparse
import json
import os
import sys

DEFAULT_DIR = r"D:/IntrusiveAnomalies/StackOBot/Saved/AnomalyCaptures/manual"


def main():
    ap = argparse.ArgumentParser(description="Overlay AnomalyInjector capture bboxes onto frames.")
    ap.add_argument("--dir", default=DEFAULT_DIR, help="capture dir containing labels.jsonl + frames")
    ap.add_argument("--out", default=None, help="output dir for annotated frames (default: <dir>/annotated)")
    args = ap.parse_args()

    cap_dir = os.path.abspath(args.dir)
    out_dir = os.path.abspath(args.out) if args.out else os.path.join(cap_dir, "annotated")
    sidecar = os.path.join(cap_dir, "labels.jsonl")

    if not os.path.isfile(sidecar):
        sys.exit(f"ERROR: no labels.jsonl in {cap_dir}\n"
                 f"       (run IAI.Capture.Shot in-game first, or pass --dir)")

    try:
        from PIL import Image, ImageDraw, ImageFont
    except ImportError:
        sys.exit("ERROR: Pillow is required.  pip install pillow")

    os.makedirs(out_dir, exist_ok=True)
    try:
        font = ImageFont.truetype("arial.ttf", 18)
    except Exception:
        font = ImageFont.load_default()

    manifest_path = os.path.join(cap_dir, "run.json")
    if os.path.isfile(manifest_path):
        with open(manifest_path, "r", encoding="utf-8") as mf:
            m = json.load(mf)
        print(f"\nrun.json: seed={m.get('seed')} K={m.get('settle_frames')} L={m.get('view_lag_frames')} "
              f"pre={m.get('pre_frames')} positive={m.get('positive_frames')} post={m.get('post_frames')} "
              f"bursts={m.get('burst_count')} fmt={m.get('format')} viewport={m.get('viewport')}")

    n_frames = 0
    n_boxes = 0
    n_present = 0
    n_present_nobox = 0
    prev_present = None
    print(f"\ncapture dir : {cap_dir}")
    print(f"annotated   : {out_dir}\n")
    print(f"{'frame':>12}  {'present':>7}  {'w x h':>11}  anomalies")
    print("-" * 78)

    with open(sidecar, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            rec = json.loads(line)
            n_frames += 1

            img_name = rec.get("image", "")
            img_path = os.path.join(cap_dir, img_name)
            present = rec.get("anomaly_present", False)
            W = rec.get("width", 0)
            H = rec.get("height", 0)
            anoms = rec.get("anomalies", [])

            summary = ", ".join(
                f"{a.get('id')}->{a.get('target_name')}"
                f"[{'box' if a.get('bbox_valid') else 'NObox'}]"
                for a in anoms
            ) or "(none)"
            any_box = any(a.get("bbox_valid") for a in anoms)
            if present:
                n_present += 1
                if not any_box:
                    n_present_nobox += 1
            flip = "  <-- present flips" if (prev_present is not None and present != prev_present) else ""
            prev_present = present
            print(f"{rec.get('frame_index',''):>12}  {str(present):>7}  {W:>4} x {H:<4}  {summary}{flip}")

            if not os.path.isfile(img_path):
                print(f"             ! image missing: {img_name}")
                continue

            im = Image.open(img_path).convert("RGB")
            draw = ImageDraw.Draw(im)
            for a in anoms:
                x, y, w, h = a.get("bbox_px", [0, 0, 0, 0])
                valid = a.get("bbox_valid", False)
                color = (255, 40, 40) if valid else (140, 140, 140)
                if w > 0 and h > 0:
                    draw.rectangle([x, y, x + w, y + h], outline=color, width=3)
                    n_boxes += 1 if valid else 0
                label = f"{a.get('id')}  {a.get('target_name')}"
                ty = max(0, y - 22)
                draw.text((x + 2, ty), label, fill=color, font=font)

            out_path = os.path.join(out_dir, os.path.splitext(img_name)[0] + "_annotated.png")
            im.save(out_path)

    print("-" * 78)
    n_visible_pos = n_present - n_present_nobox
    print(f"{n_frames} frame(s), {n_boxes} valid box(es) drawn.")
    print(f"present=True: {n_present}  |  visible-positive (present + a box): {n_visible_pos}  |  "
          f"present-but-off-screen (no box): {n_present_nobox}")
    print(f"Open the annotated frames in: {out_dir}\n")


if __name__ == "__main__":
    main()
