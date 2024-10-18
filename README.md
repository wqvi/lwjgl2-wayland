# Wayland LWJGL2.9.3 patch
I used the SDL2 library in place of the X11 library for windowing, mouse, and keyboard. This way I can play Minecraft without having to use xorg-xwayland. I tend to run into issues using Xwayland such as blurry windows or strange input. Now you can enjoy classic versions of Minecraft without the need for Xwayland!

# How to use this instead of the default lwjgl instance
If you use MultiMC or PrismLauncher you can specify java command line arguments. LWJGL has a way to set the native library loaded. Query it through the -D flag and org.lwjgl.librarypath. Assign org.lwjgl.librarypath to where ever the native library resides. An example:
`
-Dorg.lwjgl.librarypath=~/lwjgl2-wayland/libs/linux
`.
Run the python script. First parameter is the Minecraft instance directory. The second parameter is the repository location (probably will be removed soon). An example: 
`./lwjgl.py ~/.local/share/PrismLauncher/instances/1.8.9/ ~/lwjgl2-wayland/`

## Incompatible Minecraft versions
I've tested these versions of Minecraft to see which ones are incompatible with my LWJGL2 patch. However if you play with a mod that removes the Minecraft applet or if you manage to get [Wakefield](https://github.com/openjdk/wakefield) to work for Java 8 these versions will work. Some examples of these versions working are [Better than Wolves](https://github.com/BTW-Community) for Minecraft 1.5.2 and [Better than Adventure](https://www.betterthanadventure.net/).
- 1.0
- 1.1
- 1.2.5
- 1.3.2
- 1.4.7
- 1.5.1

LWJGL - Lightweight Java Game Library
======

The Lightweight Java Game Library (LWJGL) is a solution aimed directly at professional and amateur Java programmers alike to enable commercial quality games to be written in Java. 
LWJGL provides developers access to high performance crossplatform libraries such as OpenGL (Open Graphics Library), OpenCL (Open Computing Language) and OpenAL (Open Audio Library) allowing for state of the art 3D games and 3D sound.
Additionally LWJGL provides access to controllers such as Gamepads, Steering wheel and Joysticks.
All in a simple and straight forward API.

Website: [http://lwjgl.org](http://lwjgl.org)
Forum: [http://lwjgl.org/forum](http://lwjgl.org/forum)
Bugs/Suggestions: [https://github.com/LWJGL/lwjgl/issues](https://github.com/LWJGL/lwjgl/issues)

Compilation
-----------

LWJGL requires JDK-8 and Ant installed to compile, as well as your platforms native compiler, such as GCC, to compile the JNI. Additionally you will need to install the SDL2 and libepoxy packages.

```
user@host ~/lwjgl2-wayland $ ant
```
