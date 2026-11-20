# A6 — Open the Paris project once in the old 4.25 editor

**What this is:** the first time the Paris project opens in the old 4.25 editor with our plugin inside. You open it, look at one model for a minute, and close it. The kit runs all the safety checks for you.

**Time:** about 1 to 1½ hours, mostly waiting. Your hands-on part is about 5 minutes.

## Before you start
- **Save your work.**
- **Close these programs:**
  - Blender;
  - any <other-home-project> or render jobs;
  - the **Epic Games Launcher** (right-click its tray icon → Exit);
  - every other Unreal editor or game, including the UE 5.7 <other-home-project> one.
- **Pause the <other-home-project> watcher** if it's running. Leave the GDP watcher alone.

## Steps
1. **Double-click** `D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat` **once.** A black window opens. **Keep it open the whole time.**
2. It says **`Attempt 1 of 3: checking protected files, binaries and routing.`** This takes **about 13–15 minutes.** You can keep using the PC; just don't open heavy apps.
3. It says **`Checking for a quiet start`**. If a **`Waiting:`** line appears, it's telling you what to close:
   - `free memory is below 6 GiB` → close big apps;
   - `GPU use is above 15%` → close games, videos or renders;
   - `another Unreal or game program is running (PID …)` → close that program;
   - `cannot read …` → nothing to do; it keeps trying by itself.

   It re-checks by itself. If the PC still isn't quiet after 45 minutes, it stops without opening anything.
4. It says **`Starting the one authorized editor session.`** and the old editor opens. **The first start compiles shaders for a long time, maybe 20–40 minutes.** Wait until the shader counter at the bottom right is gone.
5. **If a box pops up at any point:**
   - `rebuild modules?` → **No**;
   - anything about converting, updating, upgrading or resaving → **No** or **Cancel**;
   - anything else unexpected → take a screenshot, then **No** or **Cancel**.
6. In the **Content Browser** (the bottom panel), open **Content → ParisStreet → Meshes**, then **double-click `SM_WindowHouse3`**. A mesh preview tab opens.
7. **Leave it open for 1 minute,** then close that tab.
8. **File → Exit.** If it asks to save anything, click **Don't Save**.
9. The black window says **`Editor closed. Tell chat what happened.`** Press any key to close it.
10. **Tell me:**
    - Did the editor open?
    - Did the shaders finish?
    - Did the mesh preview stay open for a minute?
    - Did any box or error pop up? Send screenshots.

## Never
- Never click **Play**.
- Never open the **AnomalyInjector** folder in the Content Browser.
- Never accept a convert, update, upgrade or resave prompt, and never change a setting.
- Never save anything.

## If it stops
- **`No quiet start within 45 minutes. No editor was started.`** → close heavy jobs, then double-click again. You have 3 tries in total.
- **Any line that starts with `Stopped:`** → take a screenshot and tell me. **Don't double-click again until I reply.** I'll read the details and tell you whether to retry.
- **If you double-clicked twice by accident** and the second window says `Stopped:`, close only that second window and keep the first one open.
- **If the editor crashes** → take a screenshot and tell me. Don't retry.

**After you tell me,** Code checks everything the editor did (job 094-18). Nothing else is needed from you for this step.
