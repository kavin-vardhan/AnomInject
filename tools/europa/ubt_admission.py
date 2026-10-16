import hashlib
import io
import json
from pathlib import Path
import struct
from write_ledger import JOB, dump

ENGINE=Path('D:/UESource/UnrealEngine')

def record(path):
    s=path.stat()
    return {'path':str(path),'bytes':s.st_size,'mtime_ns':s.st_mtime_ns,'sha256':hashlib.sha256(path.read_bytes()).hexdigest()}

def main():
    cache=ENGINE/'Engine/Intermediate/Build/XmlConfigCache.bin'
    assembly=ENGINE/'Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll'
    schema=ENGINE/'Engine/Saved/UnrealBuildTool/BuildConfiguration.Schema.xsd'
    stream=io.BytesIO(cache.read_bytes())
    def integer():return struct.unpack('<i',stream.read(4))[0]
    def string():
        size=0;shift=0
        while True:
            b=stream.read(1)[0];size|=(b&127)<<shift
            if not b&128:break
            shift+=7
        return stream.read(size).decode('utf-8')
    version=integer();count=integer()
    inputs=[Path(string()) for _ in range(count)]
    assert version==2
    expected=[ENGINE/'Engine/Saved/UnrealBuildTool/BuildConfiguration.xml',Path('C:/Users/kavin/AppData/Roaming/Unreal Engine/UnrealBuildTool/BuildConfiguration.xml')]
    restricted=ENGINE/'Engine/Restricted/NotForLicensees/Programs/UnrealBuildTool/BuildConfiguration.xml'
    if restricted.is_file():expected.insert(0,restricted)
    documents=Path('C:/Users/kavin/Documents/Unreal Engine/UnrealBuildTool/BuildConfiguration.xml')
    if documents.is_file():expected.append(documents)
    if inputs!=expected:raise RuntimeError('Existing XML cache input list differs; engine write risk')
    for p in [assembly,*inputs]:
        if p.stat().st_mtime_ns>cache.stat().st_mtime_ns:raise RuntimeError('Stale engine XML cache: '+str(p))
    if not schema.is_file():raise RuntimeError('Engine XML schema missing')
    rules=[]
    for name in ('UE5Rules','UE5ProgramRules'):
        dll=ENGINE/('Engine/Intermediate/Build/BuildRules/'+name+'.dll')
        manifest=dll.with_name(name+'Manifest.json')
        data=json.loads(manifest.read_text(encoding='utf-8-sig'))
        assert data['EngineVersion']=='5.1.1'
        bad=[s for s in data['SourceFiles'] if not Path(s).is_file() or Path(s).stat().st_mtime_ns>dll.stat().st_mtime_ns]
        if bad or assembly.stat().st_mtime_ns>dll.stat().st_mtime_ns:raise RuntimeError('Stale engine rules assembly '+name)
        rules.append({'assembly':record(dll),'manifest':record(manifest),'source_files':len(data['SourceFiles']),'missing_or_newer_sources':bad})
    dump(JOB/'routing/ubt51-admission.json',{'result':'PASS','cache':record(cache),'schema':record(schema),'ubt':record(assembly),'xml_inputs':[record(p) for p in inputs],'rules':rules,'scope':'Read-only preflight of the existing cache and rules. Actual no-engine-write proof is the EB1 before/after scan; no cache override or setting edit.'})
    print('5.1 existing XML cache and rules timestamps admitted; no UBT invoked.')

if __name__=='__main__':main()
