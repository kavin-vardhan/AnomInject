# Europa read card (week 1) — read-only, numbers only

**For:** the owner, at the office or over RDP. **Time:** about 45–75 min. If time is short, do **Part 1** only.

**Why:** Europa runs a modified 4.25 engine. These answers tell us early whether the port's plan still holds there, before we spend weeks on it.

**Rules (the usual):**
- Only **numbers** come home. Type them into chat as `01: 4 25 4` and so on.
- Use **1** = found / yes, **0** = not found / no, **-1** = don't know or can't tell.
- No copies, no screenshots, no file paths, no names.
- Nothing is changed, built or launched. You only open files and read them.
- All the file paths below are the standard engine layout. They start from **Europa's engine folder**, the one that contains `Engine\`.
- To check for a word: open the file in any text editor and press Ctrl+F.

---

## Part 1 — engine files (about 20–30 min)

**01. Engine version.** Open `Engine\Build\Build.version`. Type the three numbers after `MajorVersion`, `MinorVersion` and `PatchVersion`.
→ `01: <major> <minor> <patch>`

**02. Changelist.** Same file. Type the numbers after `Changelist` and `CompatibleChangelist`.
→ `02: <changelist> <compatible>`

**03 + 04. Compiler and SDK.** Open the latest build log, `Engine\Programs\UnrealBuildTool\Log.txt` (or whichever build log you normally check). Find the line that starts `Using Visual Studio ...`. Type **only the numbers**: the toolchain version, then the Windows SDK version.
→ `03: <toolchain, e.g. 14 16 27023>` · `04: <sdk, e.g. 10 0 17763 0>`

**05. Three names.**
- In `Engine\Source\Runtime\Engine\Classes\Engine\World.h`, search `OnWorldPostActorTick`, then `OnWorldTickEnd`.
- In `Engine\Source\Runtime\Engine\Public\Subsystems\Subsystem.h`, search `ShouldCreateSubsystem`.
→ `05: <1/0> <1/0> <1/0>`

**06. Plugin-descriptor words.** In `Engine\Source\Runtime\Projects\Private\ModuleDescriptor.cpp`, search each of these, in this order: `WhitelistTargets`, `BlacklistTargetConfigurations`, `TargetAllowList`, `TargetConfigurationDenyList`.
→ `06: <1/0> <1/0> <1/0> <1/0>`

**07. Render hooks.** In `Engine\Source\Runtime\Engine\Public\SceneViewExtension.h`, search each of these, in this order:
1. `PreRenderViewFamily_RenderThread`
2. `PreRenderView_RenderThread`
3. `PrePostProcessPass_RenderThread`
4. `SubscribeToPostProcessingPass`
5. `PostRenderViewFamily_RenderThread`
6. `PostRenderView_RenderThread`
→ `07: <six 1/0>`

**14. Tick parts.**
- In `Engine\Source\Runtime\Engine\Public\Subsystems\WorldSubsystem.h`, search `UTickableWorldSubsystem`.
- In `Engine\Source\Runtime\Engine\Private\Tickable.cpp`, search `IsAllowedToTick`.
- Third number: how many places in the game's own code depend on `UTickableWorldSubsystem`, if you know. Otherwise `-1`.
→ `14: <1/0> <1/0> <count or -1>`

## Part 2 — project settings (about 15 min)
Open Europa's project `Config\DefaultEngine.ini` (read only). For each setting, type the number written after `=`. If the setting isn't in the file, type `-1`.

**08. Forward shading.** `r.ForwardShading`
→ `08: <0/1/-1>`

**09. Anti-aliasing.** `r.DefaultFeature.AntiAliasing` (0 none, 1 FXAA, 2 TAA, 3 MSAA)
→ `09: <n or -1>`

**10. Resolution.** `r.ScreenPercentage` and `r.DynamicRes.OperationMode`. Also the game's usual window or output size, if you know it (width height).
→ `10: <percentage or -1> <dynres or -1> <width or -1> <height or -1>`

**11. Custom depth.** `r.CustomDepth`. Then: does the game itself use custom depth/stencil, for example for outlines or highlights? Answer 1, 0 or -1.
→ `11: <n or -1> <1/0/-1>`

**13. RHI and VR.**
- Under `[/Script/WindowsTargetPlatform.WindowsTargetSettings]`, read `DefaultGraphicsRHI`: DX11 = 0, DX12 = 1, Vulkan = 2, other = 3.
- VR or stereo used: 1 or 0.
- `r.VirtualTextures`: you said it's off, so this should be 0.
→ `13: <rhi> <vr> <vt>`

## Part 3 — only if you already know (otherwise -1)

**12.** Does the game run its simulation at a different rate from rendering (a fixed-tick simulation)? Second number: does it have its own control for that?
→ `12: <1/0/-1> <1/0/-1>`

**15.** In one small test area of the game you'd use for testing, roughly how many of each are there:
- visible static meshes with 2 or more LODs;
- movable lights;
- skeletal meshes (characters) with 2 or more LODs?
→ `15: <n> <n> <n>` (or `-1 -1 -1`)

---

**That's it.** Paste the lines back to chat whenever you're done. Nothing here blocks the home work; it de-risks Phase C and the first office test on Europa.
