The inherited idle monitor added person_idle_s >= 60 as a hard launch condition. The current
brief's absolute foreign-editor rule is stricter than its older 15-minute idle-editor exception;
neither text requires 60 seconds of keyboard idleness when no foreign editor or GPU workload
exists. After repeated keyboard-only blocks, that extra condition was removed explicitly.
Keyboard idleness remains recorded as a note. Foreign UnrealEditor/UE4Editor processes still
block absolutely, active background GPU work still blocks, all three NVIDIA samples must stay
below 10%, available physical memory must be at least 6 GiB, and both disk floors remain 50 GiB.
The runtime foreign-editor/GPU guard remains active. No anomaly selection, image, mask, label,
E1, identity or null threshold changed. The before script and stopped watcher identity are saved.
