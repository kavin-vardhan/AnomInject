from __future__ import print_function
import imp
import json
import sys
import types

u=types.ModuleType('unreal')
class System(object):
    frame=10
    @staticmethod
    def get_frame_count():return System.frame
    @staticmethod
    def get_console_variable_int_value(name):
        assert name=='r.CustomDepth'
        return 1
u.SystemLibrary=System
u.log=lambda line:None
callbacks=[];closed=[]
u.register_slate_post_tick_callback=lambda cb:callbacks.append(cb) or 123
u.unregister_slate_post_tick_callback=lambda handle:closed.append(handle)
sys.modules['unreal']=u
m=imp.load_source('observer','E:/IA_Fixtures/Outline425/Scripts/capture_depth_read09444d.py')
data=[]
class Stream(object):
    def __enter__(self):return self
    def __exit__(self,*args):pass
    def write(self,text):data.append(json.loads(text))
m.open=lambda path,mode:Stream()
last=[-1]
m.os.path.isfile=lambda path:int(path.rsplit('frame-',1)[1].split('.')[0])<=last[0]
state=m.start('no-write-test')
assert len(data)==1 and data[0]['kind']=='startup'
for end in (79,80,80,2299,2300):
    last[0]=end;System.frame=end+100
    callbacks[0](0)
assert [x['requested_capture_index'] for x in data]==[None,80,2300]
assert [x['latest_saved_index'] for x in data]==[-1,80,2300]
assert [x['engine_frame'] for x in data]==[10,180,2400]
assert closed==[123]
assert all(x['source']=='in_process_SystemLibrary' and x['value']==1 for x in data)
print(json.dumps(dict(result='PASS',python=sys.version.split()[0],samples=data,duplicate_suppressed=True,before_threshold_suppressed=True)))
