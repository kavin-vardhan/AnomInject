# A6 owner card draft — run only when chat says

Chat will first prepare the launch kit for A6. Do not run it before that message.

1. Save your work and close Blender and any other heavy jobs. Close other Unreal editors and games. The preflight needs at least 6 GiB free memory. Leave the mouse and keyboard alone for five minutes before the quiet-start check.
2. Double-click `D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat`. Leave its console window open. It checks the protected files, bound binaries and disk routing, then waits for a quiet start. If it reports a stop, send that message to chat and stop here.
3. The first editor start may compile shaders for tens of minutes. Wait until the shader count finishes. Do not start Play.
4. Answer **No** to any **rebuild modules?** prompt. Never accept a convert, update or resave prompt. Never open anything in the plugin's own Content folder.
5. In the Content Browser, go to `/Game/ParisStreet/Meshes` and double-click `SM_WindowHouse3`. Its full asset path is `/Game/ParisStreet/Meshes/SM_WindowHouse3.SM_WindowHouse3`. Leave the mesh-preview tab open for **60 seconds**, then close that tab.
6. Close the editor using **File > Exit**. Do not save any changed asset or project setting.
7. Tell chat whether the editor opened, whether shader compilation finished, whether the mesh preview opened for 60 seconds, and whether any dialog or error appeared. Keep the console and logs on D: for the evidence check.

If any unexpected dialog or error appears, take a screenshot, choose **No** or **Cancel**, then close the editor. Do not accept a rebuild or change settings to work around it.

This hands-on preview proves an EditorPreview world was observed. It is separate from the later measured PIE tests, which require a new quiet window. A5 has tested this kit only in dry-run mode; it has not launched the editor.
