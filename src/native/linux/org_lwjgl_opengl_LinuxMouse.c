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
 * $Id: org_lwjgl_input_Keyboard.c 2399 2006-06-30 19:28:00Z elias_naur $
 *
 * Linux mouse handling.
 *
 * @author elias_naur <elias_naur@users.sourceforge.net>
 * @version $Revision: 2399 $
 */

#define NO_SDL_GLEXT
#include <SDL2/SDL.h>
#include "common_tools.h"
#include "org_lwjgl_opengl_LinuxMouse.h"

extern SDL_Event event;

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxMouse_nGetWindowHeight(JNIEnv *env, jclass unused, jlong display_ptr, jlong window_ptr) {
	if (window_ptr == 1) {
		SDL_DisplayMode dm;
		SDL_GetCurrentDisplayMode(0, &dm);
		return dm.h;
	}

	SDL_Window *window = (SDL_Window *)window_ptr;
	int width;
	int height;
	SDL_GL_GetDrawableSize(window, &width, &height);
	return height;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxMouse_nGetWindowWidth(JNIEnv *env, jclass unused, jlong display_ptr, jlong window_ptr) {
	if (window_ptr == 1) {
		SDL_DisplayMode dm;
		SDL_GetCurrentDisplayMode(0, &dm);
		return dm.w;
	}

	SDL_Window *window = (SDL_Window *)window_ptr;
	int width;
	int height;
	SDL_GL_GetDrawableSize(window, &width, &height);
	return width;
}

JNIEXPORT void JNICALL Java_org_lwjgl_opengl_LinuxMouse_nWarpCursor(JNIEnv *env, jclass unused, jlong display_ptr, jlong window_ptr, jint x, jint y) {
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxMouse_nQueryPointer(JNIEnv *env, jclass unused, jlong display_ptr, jlong window_ptr, jobject result_buffer) {
	return window_ptr;
}

JNIEXPORT void JNICALL Java_org_lwjgl_opengl_LinuxMouse_nSendWarpEvent(JNIEnv *env, jclass unusued, jlong display_ptr, jlong window_ptr, jlong warp_atom_ptr, jint x, jint y) {
	/*Atom warp_atom = (Atom)warp_atom_ptr;
	Display *disp = (Display *)(intptr_t)display_ptr;
	Window win = (Window)window_ptr;
	XEvent warp_event;
	warp_event.type = ClientMessage;
	warp_event.xclient.window = win;
	warp_event.xclient.message_type = warp_atom;
	warp_event.xclient.format = 32;
	warp_event.xclient.data.l[0] = x;
	warp_event.xclient.data.l[1] = y;
	XSendEvent(disp, win, False, 0, &warp_event);*/
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxMouse_nGetButtonCount(JNIEnv *env, jclass unused, jlong display_ptr) {
	/*Display *disp = (Display *)(intptr_t)display_ptr;

	int count = 256;

	unsigned char * pointer_map = malloc(sizeof(unsigned char) * count);
	count = XGetPointerMapping(disp, pointer_map, count);

	free(pointer_map);

	return count;*/
	return 6; 
}
