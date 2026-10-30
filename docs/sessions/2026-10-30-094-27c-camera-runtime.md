# 094-27c — camera and runtime fixes

FI2–FI5 implementation and prediction: docs/europa/09427c-runtime-authoring-amendment.md. Qualification and real sessions are pending. No build, push or tags.

## Actual result

The two A6 maps and shared mesh were quarantined and authored fresh; both camera checks passed before any save. Keyword-only Rotator checks, the real-signature Python 2.7 stub, callback re-entry/single-shot exit and crash counting passed qualification. The prior Niagara shutdown crash root cause remains unconfirmed. Runtime sessions: 18 of 18; crashes: 1. Gates: A3 PASS, A5 FAIL, A6 FAIL, A7 FAIL, A8 FAIL. Counted-event contact sheets: 1. Initial exit canary scan: GAP, 4 unreadable paths outside the tested exclusions, no matches. Raw files and generated reader verdicts are retained. No native build, push or tags.

{
  "authoring": [
    {
      "session": "EA_Core",
      "started_utc": "2026-10-30T13:41:46.404661+00:00",
      "ended_utc": "2026-10-30T13:42:23.018757+00:00",
      "duration_seconds": 36.614096,
      "exit_code": 0,
      "result": "AUTHORING_PASS",
      "camera": {
        "tolerance_degrees": 0.01,
        "actual": {
          "yaw": 0.0,
          "roll": 0.0,
          "pitch": -12.0
        },
        "normalized": {
          "yaw": 0.0,
          "roll": 0.0,
          "pitch": -12.0
        },
        "passed": true,
        "expected": {
          "yaw": 0.0,
          "roll": 0.0,
          "pitch": -12.0
        },
        "checked_before_any_save": true
      }
    },
    {
      "session": "EA_Travel",
      "started_utc": "2026-10-30T13:58:26.697555+00:00",
      "ended_utc": "2026-10-30T13:58:39.048291+00:00",
      "duration_seconds": 12.350736,
      "exit_code": 0,
      "result": "AUTHORING_PASS",
      "camera": {
        "tolerance_degrees": 0.01,
        "actual": {
          "yaw": 0.0,
          "roll": 0.0,
          "pitch": -12.0
        },
        "normalized": {
          "yaw": 0.0,
          "roll": 0.0,
          "pitch": -12.0
        },
        "passed": true,
        "expected": {
          "yaw": 0.0,
          "roll": 0.0,
          "pitch": -12.0
        },
        "checked_before_any_save": true
      }
    }
  ],
  "runtime": [
    {
      "leg": "Smoke",
      "started_utc": "2026-10-30T14:18:42.902030+00:00",
      "ended_utc": "2026-10-30T14:18:55.699374+00:00",
      "duration_seconds": 12.797344,
      "exit_code": 0,
      "result": "PASS",
      "reader": "PASS",
      "reader_reason": null,
      "recorded_precheck_wait_seconds": 0.0,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\Smoke\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\Smoke-precheck.jsonl"
    },
    {
      "leg": "Load",
      "started_utc": "2026-10-30T14:20:58.265823+00:00",
      "ended_utc": "2026-10-30T14:21:13.337555+00:00",
      "duration_seconds": 15.071732,
      "exit_code": 0,
      "result": "PASS",
      "reader": "PASS",
      "reader_reason": null,
      "recorded_precheck_wait_seconds": 0.0,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\Load\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\Load-precheck.jsonl"
    },
    {
      "leg": "WorldsPIE",
      "started_utc": "2026-10-30T14:23:22.231090+00:00",
      "ended_utc": "2026-10-30T14:24:36.844990+00:00",
      "duration_seconds": 74.6139,
      "exit_code": 0,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Missing clean probe stop",
      "recorded_precheck_wait_seconds": 0.0,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\WorldsPIE\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\WorldsPIE-precheck.jsonl"
    },
    {
      "leg": "WorldsGame",
      "started_utc": "2026-10-30T14:38:35.830493+00:00",
      "ended_utc": "2026-10-30T14:39:03.091176+00:00",
      "duration_seconds": 27.260683,
      "exit_code": 125,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Nonzero or stopped session",
      "recorded_precheck_wait_seconds": 609.256167,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\WorldsGame\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\WorldsGame-precheck.jsonl"
    },
    {
      "leg": "GC",
      "started_utc": "2026-10-30T14:50:55.401176+00:00",
      "ended_utc": "2026-10-30T14:51:02.066178+00:00",
      "duration_seconds": 6.665002,
      "exit_code": 3,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Nonzero or stopped session",
      "recorded_precheck_wait_seconds": 580.324429,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\GC\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\GC-precheck.jsonl"
    },
    {
      "leg": "A7_:blinking:positive",
      "started_utc": "2026-10-30T14:53:03.134832+00:00",
      "ended_utc": "2026-10-30T14:53:15.786551+00:00",
      "duration_seconds": 12.651719,
      "exit_code": 125,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Nonzero or stopped session",
      "recorded_precheck_wait_seconds": 0.0,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-blinking-positive\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-blinking-positive-precheck.jsonl"
    },
    {
      "leg": "A7_:lighting_mismatch:positive",
      "started_utc": "2026-10-30T14:55:09.410953+00:00",
      "ended_utc": "2026-10-30T14:55:34.433941+00:00",
      "duration_seconds": 25.022988,
      "exit_code": 0,
      "result": "PASS",
      "reader": "PASS",
      "reader_reason": null,
      "recorded_precheck_wait_seconds": 0.0,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-lighting_mismatch-positive\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-lighting_mismatch-positive-precheck.jsonl"
    },
    {
      "leg": "A7_:lod_corruption:positive",
      "started_utc": "2026-10-30T15:07:45.714320+00:00",
      "ended_utc": "2026-10-30T15:08:13.236117+00:00",
      "duration_seconds": 27.521797,
      "exit_code": 0,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Fewer than three valid visible restored events",
      "recorded_precheck_wait_seconds": 606.496553,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-lod_corruption-positive\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-lod_corruption-positive-precheck.jsonl"
    },
    {
      "leg": "A7_:missing_object:positive",
      "started_utc": "2026-10-30T15:20:55.337226+00:00",
      "ended_utc": "2026-10-30T15:21:25.059075+00:00",
      "duration_seconds": 29.721849,
      "exit_code": 0,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Fewer than three valid visible restored events",
      "recorded_precheck_wait_seconds": 625.013975,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-missing_object-positive\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-missing_object-positive-precheck.jsonl"
    },
    {
      "leg": "A7_:blinking:NoApply",
      "started_utc": "2026-10-30T15:33:46.421260+00:00",
      "ended_utc": "2026-10-30T15:34:13.265989+00:00",
      "duration_seconds": 26.844729,
      "exit_code": 0,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Control event invalid under FB3",
      "recorded_precheck_wait_seconds": 609.251017,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-blinking-NoApply\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-blinking-NoApply-precheck.jsonl"
    },
    {
      "leg": "A7_:lighting_mismatch:NoApply",
      "started_utc": "2026-10-30T16:03:01.536408+00:00",
      "ended_utc": "2026-10-30T16:03:27.042758+00:00",
      "duration_seconds": 25.50635,
      "exit_code": 0,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Control event invalid under FB3",
      "recorded_precheck_wait_seconds": 959.314922,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-lighting_mismatch-NoApply\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-lighting_mismatch-NoApply-precheck.jsonl"
    },
    {
      "leg": "A7_:lod_corruption:NoApply",
      "started_utc": "2026-10-30T16:15:32.243291+00:00",
      "ended_utc": "2026-10-30T16:15:52.715417+00:00",
      "duration_seconds": 20.472126,
      "exit_code": 0,
      "result": "PASS",
      "reader": "PASS",
      "reader_reason": null,
      "recorded_precheck_wait_seconds": 605.650016,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-lod_corruption-NoApply\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-lod_corruption-NoApply-precheck.jsonl"
    },
    {
      "leg": "A7_:missing_object:NoApply",
      "started_utc": "2026-10-30T16:27:58.318112+00:00",
      "ended_utc": "2026-10-30T16:28:18.879145+00:00",
      "duration_seconds": 20.561033,
      "exit_code": 0,
      "result": "PASS",
      "reader": "PASS",
      "reader_reason": null,
      "recorded_precheck_wait_seconds": 605.338994,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-missing_object-NoApply\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-missing_object-NoApply-precheck.jsonl"
    },
    {
      "leg": "A7_:blinking:SkipRestore",
      "started_utc": "2026-10-30T16:40:31.096058+00:00",
      "ended_utc": "2026-10-30T16:40:55.566798+00:00",
      "duration_seconds": 24.47074,
      "exit_code": 0,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Control event invalid under FB3",
      "recorded_precheck_wait_seconds": 610.634885,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-blinking-SkipRestore\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-blinking-SkipRestore-precheck.jsonl"
    },
    {
      "leg": "A7_:lighting_mismatch:SkipRestore",
      "started_utc": "2026-10-30T18:25:09.361204+00:00",
      "ended_utc": "2026-10-30T18:25:46.312956+00:00",
      "duration_seconds": 36.951752,
      "exit_code": 125,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Nonzero or stopped session",
      "recorded_precheck_wait_seconds": 6138.530655,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-lighting_mismatch-SkipRestore\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-lighting_mismatch-SkipRestore-precheck.jsonl"
    },
    {
      "leg": "A7_:lod_corruption:SkipRestore",
      "started_utc": "2026-10-30T19:43:34.262952+00:00",
      "ended_utc": "2026-10-30T19:43:49.585682+00:00",
      "duration_seconds": 15.32273,
      "exit_code": 125,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Nonzero or stopped session",
      "recorded_precheck_wait_seconds": 3939.475582,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-lod_corruption-SkipRestore\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-lod_corruption-SkipRestore-precheck.jsonl"
    },
    {
      "leg": "A7_:missing_object:SkipRestore",
      "started_utc": "2026-10-30T19:45:27.981905+00:00",
      "ended_utc": "2026-10-30T19:50:31.487303+00:00",
      "duration_seconds": 303.505398,
      "exit_code": 125,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Nonzero or stopped session",
      "recorded_precheck_wait_seconds": 0.0,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\A7_-missing_object-SkipRestore\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\A7_-missing_object-SkipRestore-precheck.jsonl"
    },
    {
      "leg": "Admission",
      "started_utc": "2026-10-30T20:02:00.428160+00:00",
      "ended_utc": "2026-10-30T20:12:00.798724+00:00",
      "duration_seconds": 600.370564,
      "exit_code": 125,
      "result": "FAIL",
      "reader": "FAIL",
      "reader_reason": "Nonzero or stopped session",
      "recorded_precheck_wait_seconds": 588.430844,
      "gate_receipt": "D:\\IA_Europa\\Receipts\\09427c\\runtime-attempts\\Admission\\01\\gate.jsonl",
      "precheck_receipt": "D:\\IA_Europa\\Receipts\\09427c\\Admission-precheck.jsonl"
    }
  ],
  "A7": [
    {
      "leg": "A7_:blinking:positive",
      "reader": "FAIL",
      "anomaly": "blinking",
      "control": "positive",
      "attempted": 0,
      "applied": 0,
      "reverted": 0,
      "refused": 0,
      "counted": 0,
      "invalid": null,
      "valid": null,
      "accounting_source": "Raw command_result rows only; the picture reader did not complete, so validity is unassessed and no event is qualified"
    },
    {
      "leg": "A7_:lighting_mismatch:positive",
      "reader": "PASS",
      "anomaly": "lighting_mismatch",
      "control": "positive",
      "attempted": 8,
      "applied": 6,
      "reverted": 6,
      "refused": 2,
      "counted": 5,
      "invalid": 1,
      "valid": 5
    },
    {
      "leg": "A7_:lod_corruption:positive",
      "reader": "FAIL",
      "anomaly": "lod_corruption",
      "control": "positive",
      "attempted": 8,
      "applied": 6,
      "reverted": 6,
      "refused": 2,
      "counted": 0,
      "invalid": 0,
      "valid": 6
    },
    {
      "leg": "A7_:missing_object:positive",
      "reader": "FAIL",
      "anomaly": "missing_object",
      "control": "positive",
      "attempted": 8,
      "applied": 6,
      "reverted": 6,
      "refused": 2,
      "counted": 0,
      "invalid": 0,
      "valid": 6
    },
    {
      "leg": "A7_:blinking:NoApply",
      "reader": "FAIL",
      "anomaly": "blinking",
      "control": "NoApply",
      "attempted": 1,
      "applied": 0,
      "reverted": 0,
      "refused": 1,
      "counted": 0,
      "invalid": 1,
      "valid": 0
    },
    {
      "leg": "A7_:lighting_mismatch:NoApply",
      "reader": "FAIL",
      "anomaly": "lighting_mismatch",
      "control": "NoApply",
      "attempted": 1,
      "applied": 0,
      "reverted": 0,
      "refused": 1,
      "counted": 0,
      "invalid": 1,
      "valid": 0
    },
    {
      "leg": "A7_:lod_corruption:NoApply",
      "reader": "PASS",
      "anomaly": "lod_corruption",
      "control": "NoApply",
      "attempted": 1,
      "applied": 0,
      "reverted": 0,
      "refused": 1,
      "counted": 0,
      "invalid": 0,
      "valid": 1
    },
    {
      "leg": "A7_:missing_object:NoApply",
      "reader": "PASS",
      "anomaly": "missing_object",
      "control": "NoApply",
      "attempted": 1,
      "applied": 0,
      "reverted": 0,
      "refused": 1,
      "counted": 0,
      "invalid": 0,
      "valid": 1
    },
    {
      "leg": "A7_:blinking:SkipRestore",
      "reader": "FAIL",
      "anomaly": "blinking",
      "control": "SkipRestore",
      "attempted": 1,
      "applied": 1,
      "reverted": 0,
      "refused": 0,
      "counted": 0,
      "invalid": 1,
      "valid": 0
    },
    {
      "leg": "A7_:lighting_mismatch:SkipRestore",
      "reader": "FAIL",
      "anomaly": "lighting_mismatch",
      "control": "SkipRestore",
      "attempted": 0,
      "applied": 0,
      "reverted": 0,
      "refused": 0,
      "counted": 0,
      "invalid": null,
      "valid": null,
      "accounting_source": "Raw command_result rows only; the picture reader did not complete, so validity is unassessed and no event is qualified"
    },
    {
      "leg": "A7_:lod_corruption:SkipRestore",
      "reader": "FAIL",
      "anomaly": "lod_corruption",
      "control": "SkipRestore",
      "attempted": 0,
      "applied": 0,
      "reverted": 0,
      "refused": 0,
      "counted": 0,
      "invalid": null,
      "valid": null,
      "accounting_source": "Raw command_result rows only; the picture reader did not complete, so validity is unassessed and no event is qualified"
    },
    {
      "leg": "A7_:missing_object:SkipRestore",
      "reader": "FAIL",
      "anomaly": "missing_object",
      "control": "SkipRestore",
      "attempted": 0,
      "applied": 0,
      "reverted": 0,
      "refused": 0,
      "counted": 0,
      "invalid": null,
      "valid": null,
      "accounting_source": "Raw command_result rows only; the picture reader did not complete, so validity is unassessed and no event is qualified"
    }
  ]
}

The GC leg crashed on the probe's NewObject<UObject>(GetTransientPackage()) at EuropaPhaseAProbeDriver.cpp:437. Runtime.log:715 records the abstract-class assertion and :727 identifies that caller. Stock Object.h:49 marks UObject abstract; UObjectGlobals.cpp:2282 rejects it outside editor mode. This is a confirmed native probe defect; no native source or binary was changed in this job.
