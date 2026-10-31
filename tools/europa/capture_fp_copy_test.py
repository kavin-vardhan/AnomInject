import json
from pathlib import Path
import re
import hashlib

P=Path(__file__).resolve().parents[2]
E=Path('E:/EpicGames/UE_4.25/Engine/Source/Runtime')
J=Path('D:/IA_Europa/Receipts/09433b')

def accept(source):
    m=re.search(r'FRDGBufferDesc TableDesc = FRDGBufferDesc::CreateBufferDesc\(4,1536\);(.*?)Graph.CreateBuffer\(TableDesc,',source,re.S)
    usage={'BUF_ShaderResource','BUF_UnorderedAccess'}
    if m:
        for flag in re.findall(r'TableDesc.Usage \|= (BUF_\w+);',m.group(1)):usage.add(flag)
    if 'BUF_SourceCopy' not in usage:raise ValueError('4.25 staging source missing BUF_SourceCopy')
    return sorted(usage)

def run():
    engine=(E/'Windows/D3D11RHI/Private/D3D11Commands.cpp').read_text()
    readback=(E/'RHI/Private/RHIGPUReadback.cpp').read_text()
    assert '(SourceBufferRHI->GetUsage() & BUF_SourceCopy) != 0' in engine
    assert 'RHICmdList.CopyToStagingBuffer(SourceBuffer,' in readback
    source=(P/'Source/AnomalyCapture/Private/Compat/AnomalyMasks425.cpp').read_text()
    usage=accept(source)
    old=source.replace('TableDesc.Usage |= BUF_SourceCopy;','')
    assert old!=source
    try:accept(old)
    except ValueError as error:refusal=str(error)
    else:raise AssertionError('Missing copy-source usage accepted')
    from capture_fp_tests import descriptor
    descriptor(source)
    from rotator_contract import check_source
    assert not check_source(Path(__file__).read_text(),__file__)
    result=dict(result='PASS',usage=usage,negative=refusal,
        evidence='c3-attempts/C3/01/session/Runtime.log:642-643',
        citations=['D3D11Commands.cpp:2311-2320','RHIGPUReadback.cpp:71','RHIDefinitions.h:1097'],
        prediction='Retry clears the handled copy-source ensure. Frozen coverage remains expected to fail; no colour/reference/threshold change.')
    output=J/'tests/copy-source.json'
    output.write_text(json.dumps(result,indent=2))
    proof=dict(result='PASS',quick_testable_fix=True,
        diagnosis='First C3 logged a handled D3D11 ensure at frame 94: reduction readback source lacks BUF_SourceCopy. Add that usage flag; independent descriptor negative reproduces its refusal. This qualifies the sole retry for the new observed descriptor defect, not a relaxation of FO3.',
        tests=[dict(path=str(output),size=output.stat().st_size,sha256=hashlib.sha256(output.read_bytes()).hexdigest())])
    (J/'retry-qualification.json').write_text(json.dumps(proof,indent=2))
    print(json.dumps(result))

if __name__=='__main__':run()
