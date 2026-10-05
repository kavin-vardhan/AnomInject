# First runtime observation

The new three-layer fixture was authored successfully. `census-stacks-view-01` launched under a clear gate with no foreign editor or heavy GPU process during its entire lifetime. Before the new VT diagnostic command ran, its warmup command `IAI.TexCorrupt.Census allreasons` triggered D3D12 CopyCommandList Close E_INVALIDARG (frame 911). GPU budget was 7250 MB, used 3126 MB. No pixel/count result exists for this failed leg. Cause is not established. A same-source D3D debug-device run is the discriminator; no threshold or gate is being relaxed.
