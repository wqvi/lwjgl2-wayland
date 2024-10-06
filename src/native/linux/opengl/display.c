/* 
 * Copyright (c) 2002-2008 LWJGL Project
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 * * Redistributions of source code must retain the above copyright 
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * * Neither the name of 'LWJGL' nor the names of 
 *   its contributors may be used to endorse or promote products derived 
 *   from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * $Id$
 *
 * Linux specific library for display handling.
 *
 * @author elias_naur <elias_naur@users.sourceforge.net>
 * @version $Revision$
 */

#define NO_SDL_GLEXT
#include <SDL2/SDL.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common_tools.h"
#include "org_lwjgl_opengl_LinuxDisplay.h"

extern SDL_Window *context_window;

#define BPP (24)

JNIEXPORT jobjectArray JNICALL Java_org_lwjgl_opengl_LinuxDisplay_nGetAvailableDisplayModes(JNIEnv *env, jclass clazz, jlong display, jint screen, jint extension) {
	jclass displayModeClass = (*env)->FindClass(env, "org/lwjgl/opengl/DisplayMode");
	jobjectArray ret = (*env)->NewObjectArray(env, 1, displayModeClass, NULL);
	jmethodID displayModeConstructor = (*env)->GetMethodID(env, displayModeClass, "<init>", "(IIII)V");

	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);

	jobject displayMode = (*env)->NewObject(env, displayModeClass, displayModeConstructor, dm.w, dm.h, BPP, dm.refresh_rate);
	(*env)->SetObjectArrayElement(env, ret, 0, displayMode);
	return ret;
}

JNIEXPORT void JNICALL Java_org_lwjgl_opengl_LinuxDisplay_nSwitchDisplayMode(JNIEnv *env, jclass clazz, jlong display, jint screen, jint extension, jobject mode) {
	jclass cls_displayMode = (*env)->GetObjectClass(env, mode);
	jfieldID fid_fullscreen = (*env)->GetFieldID(env, cls_displayMode, "fullscreen", "Z");

	int fullscreen = (*env)->GetBooleanField(env, mode, fid_fullscreen);
	if (fullscreen) {
		SDL_SetWindowFullscreen(context_window, SDL_WINDOW_FULLSCREEN);
		SDL_SetRelativeMouseMode(SDL_FALSE);
	} else {
		int flags = SDL_GetWindowFlags(context_window);
		// make sure that it won't accidentally be set to floating
		if (flags & SDL_WINDOW_FULLSCREEN) {
			SDL_SetWindowFullscreen(context_window, 0);
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}
	}
}
