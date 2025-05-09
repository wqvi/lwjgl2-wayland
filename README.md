# LWJGL2 Wayland patch
The SDL2 library is used in place of the X11 library for windowing, mouse, and keyboard. Enjoy older versions of Minecraft without the need for XWayland!

***Complete removal of X11 components is incomplete.***

## Package Requirements
- SDL2
- libepoxy
- GCC/CLANG
- Ant
- Java 8
- LibXt *(To be removed)*

## Compilation
First and foremost compile the jar file and native libraries using ant.
```
$ ant all
```

## Patching the LWJGL library used
Using MultiMC or PrismLauncher open the console window of your Minecraft instance of choice. Go to the version tab, choose the LWJGL2 item in the list. Then click customize, this will generate a JSON file in the instance's patches directory.

### Example usage
1. Open 1.8.9 console window
2. Go to version tab
3. Select the LWJGL2 item in the list.
4. Click customize
5. Then run the script from your command line from the git repository directory
```
$ ./lwjgl.py ~/.local/share/PrismLauncher/instances/1.8.9/
```

## Broken versions
Anything prior to version _1.5.x_ has an a Java applet which relies on X11. In order to fix this the applet needs to be removed or by updating the Java applet to use Wayland.

### Applet removal
Some mods remove the Java applet which allows the game to run natively again.
- [Better than Wolves](https://github.com/BTW-Community) for Minecraft 1.5.2
- [Better than Adventure](https://www.betterthanadventure.net/) for Minecraft Beta 1.7.3

## Hyprland freezing workaround
If you're using Hyprland and experience freezing when opening the inventory, chat, or pause menu while in game, it's likely due to key repeat events under Wayland.

As a workaround, you can disable key repeats for the game process by querying for the active window, then setting `repeat_rate=0` as long as Minecraft is that active window. `mc-repeat-toggle.sh` is a script that does all of this for you! You may need to tweak the script depending on what version of Minecraft you are using, but don't worry, it is very self-explanatory.

Make the script executable and run it however you want (manual, systemd service, exec-once, etc.). You should no longer experience freezes. This is a temporary workaround, and will be fixed in a future version.

# Links to original LWJGL
Website: [http://lwjgl.org](http://lwjgl.org)
Forum: [http://lwjgl.org/forum](http://lwjgl.org/forum)
Bugs/Suggestions: [https://github.com/LWJGL/lwjgl/issues](https://github.com/LWJGL/lwjgl/issues)
