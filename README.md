![day](doc/day.png)
![studio](doc/studio.png)
![night](doc/night.png)

#### Building
*   Clone [GLFW 3.3.2](https://github.com/glfw/glfw/releases/tag/3.3.2) into directory `external/glfw-3.3.1`.
*   Clone [glad v0.1.33](https://github.com/Dav1dde/glad/releases/tag/v0.1.33) into directory `external/glad-0.1.33`.
*   Clone [GLM 0.9.9.8](https://github.com/g-truc/glm/releases/tag/0.9.9.8) into directory `external/glm-0.9.9.8`.
*   Clone [stb](https://github.com/nothings/stb) into directory `external/stb`.
*   Build using CMake.

#### Controls
*   Drag MMB to orbit around the camera's focal point.
*   Drag RMB to move the camera's focal point in the XZ-plane.
*   Click on scene objects to bring up the translation widget.
*   Click on and drag with LMB on the translation widget to translate the scene object.
*   Scroll to change the distance of the camera to its focal point.
*   Press 1, 2, or 3 for an environment change.
*   Press F1 or F2 for a scene change.

#### Future improvements
*   Texture streaming: load higher quality textures on a separate thread.
*   Generate cubemap, irradiance cubemap, pre-filter cubemap, and BRDF LUT at compile time rather than run time.

#### Image credit
*   Material textures obtained from [TextureHaven](https://texturehaven.com/).
*   HDR textures obtained from [HDRIHaven](https://hdrihaven.com/).