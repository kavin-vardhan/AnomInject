#!/usr/bin/env python3
"""Audit schema-3 capture artifacts without modifying a session.

Default mode requires --reference: a deterministic, frame-aligned control capture
with identical camera/game state and injection disabled. Adjacent video frames
are NOT a valid control. Pixel differences establish change against that control,
not human recognizability or causality in arbitrary independently recorded games.

Use --integrity-only for file/label/mask consistency without visual certification.
Dependencies: Pillow, numpy. Exit 0: requested checks passed; 1: failed;
2: visual verification unavailable. Output is JSON, suitable for delivery gates.
"""
import argparse
import json
from pathlib import Path

import numpy as np
from PIL import Image


def audit(directory, reference=None, *, integrity_only=False, pixel_delta=8, min_changed_pixels=1):
    directory = Path(directory).resolve()
    errors = []
    unknown = 0
    checked_pixels = 0

    def check(condition, message):
        if not condition:
            errors.append(message)

    def local_path(name):
        path = (directory / name).resolve()
        if not path.is_relative_to(directory):
            raise ValueError(f"Artifact path escapes session: {name}")
        return path

    annotation = json.loads((directory / "annotation.json").read_text(encoding="utf-8-sig"))
    check(annotation.get("label_schema") == 3, "Expected label_schema 3")
    check(annotation.get("capture_complete") is True, "Capture is incomplete")
    rows = {}
    for line in (directory / "labels.jsonl").read_text(encoding="utf-8-sig").splitlines():
        row = json.loads(line)
        index = row["session_index"]
        check(index not in rows, f"Duplicate label row {index}")
        rows[index] = row
    requested = annotation["requested_frames"]
    expected = set(range(requested))
    check(set(rows) == expected, "Missing or unexpected label rows")
    check(annotation.get("written_frame_indices") == sorted(rows), "Written-frame manifest disagrees with labels")
    check(annotation["video"]["total_frames"] == len(rows), "Video frame count disagrees with labels")
    check(len(rows) > 0, "Empty session cannot pass verification")

    events = {event["event_id"]: event for event in annotation["anomalies"]}
    check(len(events) == len(annotation["anomalies"]), "Duplicate event IDs")
    observed = {}
    images = set()
    masks = set()
    for index, row in sorted(rows.items()):
        check(row.get("schema_version") == 3, f"Unexpected frame schema at {index}")
        check(row.get("injection_present") == bool(row["anomalies"]), f"Injection membership disagrees at {index}")
        name = row["image"]
        check(name not in images, f"Image reused by multiple frames: {name}")
        images.add(name)
        path = local_path(name)
        check(path.is_file(), f"Missing image {name}")
        if not path.is_file():
            continue
        with Image.open(path) as image:
            rgb = np.asarray(image.convert("RGB"))
        height, width = rgb.shape[:2]
        check((width, height) == (row["width"], row["height"]), f"Image dimensions disagree at {index}")
        check([width, height] == annotation["video"]["resolution"], f"Video dimensions disagree at {index}")
        mask_name = row.get("mask_file")
        mask = None
        if row.get("mask_state") == "present":
            check(bool(mask_name), f"Present mask has no filename at {index}")
            if mask_name:
                masks.add(mask_name)
                mask_path = local_path(mask_name)
                check(mask_path.is_file(), f"Missing mask {mask_name}")
                if mask_path.is_file():
                    with Image.open(mask_path) as image:
                        check(image.mode == "L", f"Mask must be 8-bit grayscale at {index}")
                        mask = np.asarray(image.convert("L"))
                    check(mask.shape == (height, width), f"Mask dimensions disagree at {index}")
                    check(bool(np.any(mask)), f"Present mask is empty at {index}")
        else:
            check(mask_name is None, f"Non-present mask names a file at {index}")

        changed = None
        if reference is not None:
            control_path = Path(reference) / name
            check(control_path.is_file(), f"Missing control frame {index}")
            if control_path.is_file():
                with Image.open(control_path) as image:
                    control = np.asarray(image.convert("RGB"))
                check(control.shape == rgb.shape, f"Control dimensions disagree at {index}")
                if control.shape == rgb.shape:
                    changed = np.max(np.abs(rgb.astype(np.int16) - control.astype(np.int16)), axis=2) >= pixel_delta

        positive = False
        owners = {}
        row_events = set()
        for target in row["anomalies"]:
            event_id = target["event_id"]
            check(event_id not in row_events, f"Duplicate event {event_id} in frame {index}")
            row_events.add(event_id)
            check(event_id in events, f"Injected event omitted from annotation: {event_id}")
            pixels = target["target_pixels"]
            observable = target["observable"]
            check(target.get("injected") is True, f"Event membership not marked injected at {index}")
            check(observable is None or type(observable) is bool, f"Invalid observability at {index}")
            check(type(pixels) is int and pixels >= -1, f"Invalid pixel count at {index}")
            check((pixels == -1) == (observable is None) or row.get("render_state") == "shaders_pending",
                  f"Unknown measurement represented inconsistently at {index}")
            if row.get("render_state") == "shaders_pending":
                check(observable is None, f"Incomplete shaders must have unknown observability at {index}")
            if observable is None:
                unknown += 1
            positive |= observable is True
            if observable is True:
                check(pixels > 0, f"Positive without pixels at {index}")
                check(row.get("render_state") != "shaders_pending", f"Positive with incomplete shaders at {index}")
            observed.setdefault(event_id, {})[index] = (pixels, observable)
            tag = target.get("mask_value", 0)
            if tag > 0:
                check(tag not in owners or owners[tag] == event_id, f"Tag {tag} has multiple event owners at {index}")
                owners[tag] = event_id
            if pixels >= 0:
                check(tag > 0, f"Measured target has no tag at {index}")
                if mask is not None and mask.shape == (height, width):
                    actual = int(np.count_nonzero(mask == tag))
                    check(actual == pixels, f"Mask pixel count disagrees at {index}/{event_id}: {actual} != {pixels}")
                else:
                    check(pixels == 0 and row.get("mask_state") == "empty", f"Measured target lacks mask evidence at {index}")
            if changed is not None and observable is not None:
                region = None
                if mask is not None and mask.shape == changed.shape and tag > 0 and pixels > 0:
                    region = mask == tag
                elif target.get("bbox_valid"):
                    x, y, w, h = target["bbox_px"]
                    region = np.zeros(changed.shape, dtype=bool)
                    region[max(0, int(y)):min(height, int(np.ceil(y + h))),
                           max(0, int(x)):min(width, int(np.ceil(x + w)))] = True
                if region is not None and np.any(region):
                    change_visible = int(np.count_nonzero(changed & region)) >= min_changed_pixels
                    check(change_visible == observable, f"Pixel-change evidence disagrees at {index}/{event_id}")
                    checked_pixels += 1
                elif observable is True:
                    check(False, f"No auditable region for positive {index}/{event_id}")
        check(row.get("anomaly_present") == positive, f"anomaly_present disagrees at {index}")
        check(row.get("visible_positive") == positive, f"visible_positive disagrees at {index}")
        if mask is not None:
            check(set(np.unique(mask)) - {0} <= set(owners), f"Mask has unlabelled tags at {index}")

    def window(event_id, value, indices, title):
        check(value.get("frame_indices") == indices, f"{title} indices disagree for {event_id}")
        check(value.get("frame_count") == len(indices), f"{title} count disagrees for {event_id}")
        check(value.get("start_frame") == (indices[0] if indices else -1), f"{title} onset disagrees for {event_id}")
        check(value.get("end_frame") == (indices[-1] if indices else -1), f"{title} end disagrees for {event_id}")
        check(value.get("span_frame_count") == (indices[-1] - indices[0] + 1 if indices else 0), f"{title} span disagrees for {event_id}")

    for event_id, event in events.items():
        samples = observed.get(event_id, {})
        injected = sorted(samples)
        visible = sorted(i for i, (_, visible) in samples.items() if visible is True)
        window(event_id, event["injected_frames"], injected, "Injected")
        window(event_id, event["affected_frames"], visible, "Visible")
        check(event.get("manifested") == bool(visible), f"Manifested flag disagrees for {event_id}")
        check(event.get("observable_frame_count") == len(visible), f"Observable count disagrees for {event_id}")
        check(event.get("unmeasured_frame_count") == sum(v is None for _, v in samples.values()), f"Unknown count disagrees for {event_id}")
        observations = event.get("frame_observations", [])
        from_event = {o["session_index"]: (o["target_pixels"], o["observable"]) for o in observations}
        check(len(from_event) == len(observations) and from_event == samples, f"Per-frame event observations disagree for {event_id}")

    actual_images = {p.relative_to(directory).as_posix() for p in (directory / "Actual_Frames").glob("frame_*.*")}
    actual_masks = {p.relative_to(directory).as_posix() for p in (directory / "target_mask").glob("frame_*.png")}
    check(images == actual_images, "Image files and label rows do not match")
    check(masks == actual_masks, "Mask files and label references do not match (including onset/orphan files)")
    unavailable = not integrity_only and (reference is None or checked_pixels == 0 or unknown > 0)
    status = "failed" if errors else "not_verified" if unavailable else "passed"
    return {"status": status, "scope": "integrity" if integrity_only else "integrity_and_control_pixel_change",
            "frames": len(rows), "events": len(events), "unmeasured_observations": unknown,
            "pixel_comparisons": checked_pixels, "errors": errors}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("directory", type=Path)
    parser.add_argument("--reference", type=Path)
    parser.add_argument("--integrity-only", action="store_true")
    parser.add_argument("--pixel-delta", type=int, default=8)
    parser.add_argument("--min-changed-pixels", type=int, default=1)
    args = parser.parse_args()
    if not 1 <= args.pixel_delta <= 255 or args.min_changed_pixels < 1:
        parser.error("pixel-delta must be 1..255 and min-changed-pixels must be positive")
    try:
        result = audit(**vars(args))
    except (OSError, ValueError, KeyError, TypeError, IndexError) as error:
        result = {"status": "failed", "errors": [str(error)]}
    print(json.dumps(result, indent=2))
    return {"passed": 0, "failed": 1, "not_verified": 2}[result["status"]]


if __name__ == "__main__":
    raise SystemExit(main())
