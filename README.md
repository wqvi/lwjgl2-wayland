# Wayland LWJGL2.9.3 patch
I used the SDL2 library in place of the X11 library for windowing, mouse, and keyboard. This way I can play Minecraft without having to use xorg-xwayland. I tend to run into issues using Xwayland such as blurry windows or strange input. Now you can enjoy classic versions of Minecraft without the need for Xwayland!

# How to use this instead of the default lwjgl instance
If you use MultiMC or PrismLauncher you can specify java command line arguments. LWJGL has a way to set the native library loaded. Query it through the -D flag and org.lwjgl.librarypath. Assign org.lwjgl.librarypath to where ever the native library resides. An example:
`
-Dorg.lwjgl.librarypath=~/lwjgl2-wayland/libs/linux
`

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

LWJGL requires a JDK and Ant installed to compile, as well as your platforms native compiler to compile the JNI.

* ant generate-all
* ant compile
* ant compile_native
