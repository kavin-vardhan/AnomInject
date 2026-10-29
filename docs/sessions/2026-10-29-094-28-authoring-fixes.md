# 09428 authoring fixes

The Python 2.7 launcher now retains its fixture module. A recording harness exercises all 4.25 in-editor sources, the smoke path and both complete recipe sequences; the observed runpy failure is a can-fail. Both usage analytics and unattended crash reports are disabled by required command-line privacy overrides. Smoke precedes recipe authoring, records API availability without creating assets, and must prove clean exit and no upload.

FC5 managed files are recorded separately; FC6 tracks owned UBT argv and rejects build actions. The retained suites pass, with FA1 alias tests using simulated reparse attributes instead of creating links. See `docs/europa/09428-authoring-amendments.md` for source citations, scope and limits. Scratch rehearsal, re-arming and real runtime evidence are pending at this tooling commit; the final receipt index will record their outcomes. No push or tags.
