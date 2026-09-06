import copy
import json
from pathlib import Path
import sys
import tempfile
import unittest

import numpy as np
from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from verify_integrity import audit


class IntegrityTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name) / "capture"
        self.reference = Path(self.temp.name) / "control"
        (self.root / "Actual_Frames").mkdir(parents=True)
        (self.root / "target_mask").mkdir()
        (self.reference / "Actual_Frames").mkdir(parents=True)
        self.rows = []
        for i in range(2):
            name = f"Actual_Frames/frame_{i:05d}.png"
            control = np.zeros((4, 4, 3), dtype=np.uint8)
            Image.fromarray(control).save(self.reference / name)
            rgb = control.copy()
            rgb[1:3, 1:3] = 255
            Image.fromarray(rgb).save(self.root / name)
            mask = np.zeros((4, 4), dtype=np.uint8)
            mask[1:3, 1:3] = 200
            mask_name = f"target_mask/frame_{i:05d}.png"
            Image.fromarray(mask).save(self.root / mask_name)
            self.rows.append({"session_index": i, "schema_version": 3, "image": name, "width": 4, "height": 4,
                              "mask_state": "present", "mask_file": mask_name, "visible_positive": True,
                              "anomaly_present": True, "injection_present": True, "anomalies": [{"event_id": "missing_texture@5:target", "injected": True,
                              "id": "missing_texture", "target_name": "target", "target_pixels": 4,
                              "observable": True, "mask_value": 200, "bbox_valid": True, "bbox_px": [1, 1, 2, 2]}]})
        window = {"start_frame": 0, "end_frame": 1, "frame_count": 2, "span_frame_count": 2, "frame_indices": [0, 1]}
        self.annotation = {"label_schema": 3, "capture_complete": True, "requested_frames": 2,
                           "written_frame_indices": [0, 1], "video": {"total_frames": 2, "resolution": [4, 4]},
                           "anomalies": [{"event_id": "missing_texture@5:target", "affected_frames": copy.deepcopy(window),
                           "injected_frames": copy.deepcopy(window), "manifested": True, "observable_frame_count": 2,
                           "unmeasured_frame_count": 0, "frame_observations": [
                               {"session_index": i, "target_pixels": 4, "observable": True} for i in range(2)]}]}

    def tearDown(self):
        self.temp.cleanup()

    def run_audit(self, **kwargs):
        (self.root / "annotation.json").write_text(json.dumps(self.annotation), encoding="utf-8")
        (self.root / "labels.jsonl").write_text("\n".join(json.dumps(row) for row in self.rows), encoding="utf-8")
        return audit(self.root, **kwargs)

    def test_valid_integrity(self):
        self.assertEqual(self.run_audit(integrity_only=True)["status"], "passed")

    def test_missing_control_cannot_certify_pixels(self):
        self.assertEqual(self.run_audit()["status"], "not_verified")

    def test_control_pixel_change(self):
        self.assertEqual(self.run_audit(reference=self.reference)["status"], "passed")

    def test_false_positive_unchanged_image(self):
        Image.open(self.reference / self.rows[0]["image"]).save(self.root / self.rows[0]["image"])
        self.assertEqual(self.run_audit(reference=self.reference)["status"], "failed")

    def test_missing_onset_mask_reference(self):
        self.rows[0]["mask_state"] = "unmeasured"
        self.rows[0]["mask_file"] = None
        self.assertEqual(self.run_audit(integrity_only=True)["status"], "failed")

    def test_inflated_pixel_count(self):
        self.rows[0]["anomalies"][0]["target_pixels"] = 8
        self.assertEqual(self.run_audit(integrity_only=True)["status"], "failed")

    def test_missing_frame_is_not_ignored(self):
        self.rows.pop(0)
        self.assertEqual(self.run_audit(integrity_only=True)["status"], "failed")

    def test_omitted_event_is_not_ignored(self):
        self.annotation["anomalies"] = []
        self.assertEqual(self.run_audit(integrity_only=True)["status"], "failed")

    def test_duplicate_tag_owner(self):
        other = copy.deepcopy(self.rows[0]["anomalies"][0])
        other["event_id"] = "missing_texture@5:other"
        self.rows[0]["anomalies"].append(other)
        self.assertEqual(self.run_audit(integrity_only=True)["status"], "failed")

    def test_wrong_visible_end(self):
        self.annotation["anomalies"][0]["affected_frames"]["end_frame"] = 3
        self.assertEqual(self.run_audit(integrity_only=True)["status"], "failed")

    def test_incomplete_shader_cannot_be_labelled_clean(self):
        self.rows[0]["render_state"] = "shaders_pending"
        self.rows[0]["anomalies"][0]["observable"] = False
        result = self.run_audit(integrity_only=True)
        self.assertTrue(any("Incomplete shaders must have unknown" in error for error in result["errors"]))

    def test_unmeasured_retained_without_positive(self):
        for row in self.rows:
            (self.root / row["mask_file"]).unlink()
            row.update(mask_file=None, mask_state="unmeasured", visible_positive=False, anomaly_present=False)
            row["anomalies"][0].update(target_pixels=-1, observable=None, mask_value=0)
        event = self.annotation["anomalies"][0]
        event.update(manifested=False, observable_frame_count=0, unmeasured_frame_count=2)
        event["affected_frames"] = {"start_frame": -1, "end_frame": -1, "frame_count": 0, "span_frame_count": 0, "frame_indices": []}
        for obs in event["frame_observations"]:
            obs.update(target_pixels=-1, observable=None)
        self.assertEqual(self.run_audit(integrity_only=True)["status"], "passed")
        self.assertEqual(self.run_audit(reference=self.reference)["status"], "not_verified")


if __name__ == "__main__":
    unittest.main()
