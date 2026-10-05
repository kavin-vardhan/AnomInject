# 092-03c VT proxy proof harness

These are the job's actual Windows harness scripts and frozen criteria. They retain explicit
workspace, build-host and evidence paths; they are not general product commands. The original
092-03b probe remains separately preserved in `tools/proof09203`.

Work root: `E:/IA_BuildCache/_r921/09203c`. Build host:
`E:/IA_BuildCache/_r53_host/StackOBot`. Engine: `D:/UESource/UnrealEngine`.
Readers require Python, Pillow and NumPy. The immutable E1 dependency is
`E:/IA_BuildCache/_r915/harness-090-10e-eval.py`, SHA-256
`95cfb3fc1724da487fe73d371f8bedb6fcda70ea047dcad25b2933c2d7ef9962`.
Repository `tools/verify_capture.py` supplies strengthened annotation and label-rule readers.

`predictions.md` records the authorized order. `positive-criteria.md`, the inherited identity
reader, and hash receipts preserve thresholds before observations. Scheduling and fixture
corrections are recorded separately. Each launcher requires a fresh named output folder, checks
the idle/disk gate, pins its six binaries to an archive, records its PID and guards that owned
process. `run_queue.py` serializes named steps, waits five minutes between blocked checks, and
stops on an unexpected result. Never run two engine queues concurrently.

The principal commands used from the work root are:

```text
python -B launch_positive.py <new-leg> targeted 10
python -B check_positive.py <new-leg> 10
python -B launch_positive.py <new-leg> auto 3
python -B launch_staged.py <new-leg> targeted 5
python -B launch_staged.py <new-leg> auto 3
python -B launch_positive.py <new-leg> no_bind 1
python -B launch_staged.py <new-leg> baseline 1
python -B check_refusal.py <new-leg> virtual_texture
python -B launch_positive.py <new-leg> destroy 1
python -B check_lifetime.py <new-leg>
python -B launch_regression.py <new-leg> hold
python -B check_regression.py <new-leg>
```

The no-bind positive reader is expected to fail. `negative`, `constant` and `uniform` modes deliberately
replace the target's material in the temporary PIE world and require refusal; `negative` uses the
separate two-named-parameter material created by `launch_named_stack.py`. `mixed` requires
both replacement kinds in one event. `displace` and `world_end` exercise interruption and
teardown. `launch_window.py <new-leg> no_request` exercises bounded private-prefetch refusal.
Normal identity uses `launch_window.py` and `check_native.py`; the original frozen geometry uses
`launch_identity.py` and `check_identity.py`.

`cook_vt.py` invokes the existing AutomationTool DLL with no compile and directs temporary,
staging and archived outputs into this job. `launch_staged.py ... baseline` temporarily copies
the receipted release exe into only that job-owned distribution and restores it in `finally`.
Regression profile overrides are similarly receipted and removed in `finally`.

Author scripts only edit the new `ProxyVT0921C` fixture assets. The camera launcher requires an
unused alphanumeric attempt label. `restore_host.py` restores the original detached source,
six binary hashes and every backed-up setting; it removes only receipted job-created settings.
Source/build/audit gates and their can-fails are included. `copied-hashes.json` records the exact
copies. Raw sessions, failed attempts and restoration receipts stay in the evidence root and
the final relay report; a harness copy alone is not a proof verdict.

The final native identity reader requires all 480 unique session indices and uses index 100 for
its reference. `canfail_native_completeness.py` checks truncated and duplicate input. The original
identity reader and every pixel threshold remain frozen. `proof_environment.py` checks feature/host
Source, Shaders and descriptor tree equality as well as all six live binary hashes.
