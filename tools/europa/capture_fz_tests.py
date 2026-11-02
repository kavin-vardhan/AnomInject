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
    from capture_fr_tests import violations
    scanner=(root/'Source/AnomalyCapture/Private/Compat/AnomalyHostDepth425.cpp').read_text()
    assert not violations(scanner,True)
    assert violations(scanner+'ActorHasTag(TEXT("EA_BlinkCube"))',True)
    assert violations(scanner+'FIntPoint(1280,720)',True)
    assert 'BenchFixture(World)' in scanner and 'FParse::Param(FCommandLine::Get(),TEXT("IAIBench"))' in scanner
    def scanner_parity(text):
        settings=text[text.index('auto Settings='):text.index('for (TActorIterator<APostProcessVolume>')]
        return 'B.Weight' not in settings and '++R.Entries;' in settings
    assert scanner_parity(scanner)
    assert not scanner_parity(scanner.replace('++R.Entries;','if (B.Weight<=0) { continue; } ++R.Entries;'))
    result=dict(result='PASS',guard_before_street_and_masks=True,fixture_gate=True,
        zero_weight_reader_retained=True,zero_weight_skip_mutant_refused=True,
        static_fixture_exception='FZ3 package literals only in admission function; target and resolution mutants refused')
    Path(output).write_text(json.dumps(result,indent=2))
    print(json.dumps(result))


if __name__=='__main__':
    run(sys.argv[1])
