# A6 static content admission prediction

Five plugin packages are byte-unchanged: four materials and one noise texture. Each has magic 0x9e2a83c1, legacy -8, UE4 522, UE5 1008. Receipts/09411/content/plugin-package-headers.json contains full hashes. No asset was loaded.

Engine roots below: 425 = E:/EpicGames/UE_4.25/Engine/Source.

| Operation | Source basis | Prediction and A6/A8 check |
|---|---|---|
| Mount plugin content | Runtime/Projects/Private/PluginManager.cpp:715-808 | CanContainContent registers the mount path; mounting does not deserialize all packages. Mount log is expected. |
| Read UE5 package summary | Runtime/CoreUObject/Private/UObject/PackageFileSummary.cpp:85-101 | CurrentLegacyFileVersion=-7; -8 is less, sets FileVersionUE4/licensee to zero and returns before body parsing. No speculative UE5 layout read. |
| Registry discovery | Runtime/AssetRegistry/Private/PackageReader.cpp:61-83 and AssetDataGatherer.cpp:863-873 | VersionTooOld, false; zero asset rows for these five packages. Only CustomVersionMissing can retry, so these files do not enter a retry loop. |
| Thumbnails | AssetDataGatherer.cpp:864-886 | Registry rejects before ReadAssetRegistryData/ReadAssetDataFromThumbnailCache. No automatic thumbnail body load for these packages. |
| Forced load | Runtime/CoreUObject/Private/UObject/LinkerLoad.cpp:1175-1181 | Refused as an incompatible previous package version (because the summary intentionally reports zero). The wording can look older, not newer. A direct user force-load may report a missing/unloadable asset; do not force-load during A6 qualification. |
| Plugin constructor/startup | Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp constructor capability branch; disabled module implementations | On 4.25, no five material finders execute and fields stay null. Paris original predates the plugin, and copied startup maps reference no plugin material. Expect no startup load/dialog/crash from these assets; verify logs before fixture authoring. |
| A8 cooking | Same linker and registry boundaries | Explicit host-only maps should omit unused plugin content. CookAll or forced always-cook can try incompatible packages and is not admitted. Verify package output absence, not just configuration intent. |

No unconditional content isolation is needed by this prediction, and A5 changes no Content or CanContainContent flag. If A6 observes an unexpected startup load, blocking dialog or crash, stop. Proposed fallback for chat: assemble an owned 4.25 distribution/profile from the committed plugin with the five 5.1 Content packages excluded by an explicit file manifest, preserving their git blobs and keeping the 5.1 copies untouched. Alternatively a separately ruled version-specific content plugin can own later 4.25 materials. Do not delete tracked assets, edit the shared descriptor per-engine, or install the fallback without that ruling. A8's exclusion is required even if A6 startup succeeds.

Static package headers cannot prove shader readiness, lighting, LOD visibility or fork behavior. The A6 log and registry checks remain necessary. The five assets are not silently converted, resaved or patched.
