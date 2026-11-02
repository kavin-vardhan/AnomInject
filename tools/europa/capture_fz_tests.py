import json
import os
import sys
import tempfile
import types
from pathlib import Path


def run(output):
    root=Path(__file__).resolve().parents[2]
    source=(root/'Source/AnomalyCapture/Private/Compat/AnomalyCaptureLabels425.cpp').read_text()
    start=source[source.index('bool FAnomalyCaptureLabels425::Start('):source.index('void FAnomalyCaptureLabels425::Prepare(')]
    assert start.index('AnomalyHostDepth425::Admit')<start.index('Street->Start')<start.index('MaskOwner->Start')
    old=start.replace('    if (!AnomalyHostDepth425::Admit(World,Directory)) { FPlatformMisc::RequestExit(false); return false; }','')
    assert 'AnomalyHostDepth425::Admit' not in old
    street=(root/'Source/AnomalyCapture/Private/Compat/AnomalyStreetBench425.cpp').read_text()
    assert street.index('!AnomalyHostDepth425::BenchFixture(World)')<street.index('Prim->SetCustomDepthStencilValue')
    assert '!AnomalyHostDepth425::FixtureActor(A.Get())' in street
    result=dict(result='PASS',guard_before_street_and_masks=True,fixture_gate=True)
    Path(output).write_text(json.dumps(result,indent=2))
    print(json.dumps(result))


if __name__=='__main__':
    run(sys.argv[1])
