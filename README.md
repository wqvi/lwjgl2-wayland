# What is this project?
I personally use a purely wayland system. I want to play older versions of Minecraft. Unfortunately those only support X and I would rather not use Xwayland. So I decided to try my hand at patching lwjgl to work with wayland.

# I built it this way instead
This way will generate the libs/ path
* ant generate-all
* ant all

# How to use this instead of the default lwjgl instance
If you use MultiMC or PrismLauncher you can specify java command line arguments. Lwjgl has a command line argument known as org.lwjgl.librarypath. Specify this to the libs/linux path of the repository. An example:
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
