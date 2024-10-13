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
 * $Id: org_lwjgl_opengl_LinuxEvent.c 2598 2006-10-24 08:33:09Z elias_naur $
 *
 * @author elias_naur <elias_naur@users.sourceforge.net>
 * @version $Revision: 2598 $
 */

#include <jni.h>
#include "common_tools.h"
#include "org_lwjgl_opengl_LinuxEvent.h"
#define NO_SDL_GLEXT
#include <SDL2/SDL.h>

extern int is_running;
extern SDL_Window *context_window;
SDL_Event event;
SDL_Keysym keysym;

JNIEXPORT jobject JNICALL Java_org_lwjgl_opengl_LinuxEvent_createEventBuffer(JNIEnv *env, jclass unused) {
	return newJavaManagedByteBuffer(env, sizeof(SDL_Event));
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_getPending(JNIEnv *env, jclass unused, jlong display_ptr) {
	return SDL_PollEvent(&event);
}

JNIEXPORT void JNICALL Java_org_lwjgl_opengl_LinuxEvent_nSetWindow(JNIEnv *env, jclass unused, jobject event_buffer, jlong window_ptr) {
}

JNIEXPORT void JNICALL Java_org_lwjgl_opengl_LinuxEvent_nSendEvent(JNIEnv *env, jclass unused, jobject event_buffer, jlong display_ptr, jlong window_ptr, jboolean propagate, jlong eventmask) {
	/*XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	Display *disp = (Display *)(intptr_t)display_ptr;
	Window window = (Window)window_ptr;
	XSendEvent(disp, window, propagate == JNI_TRUE ? True : False, eventmask, event);*/
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetFocusDetail(JNIEnv *env, jclass unused, jobject event_buffer) {
	// lol?
	// just needs to be anything that won't trigger
	// a comparison on the java side
	return 129384;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetFocusMode(JNIEnv *env, jclass unused, jobject event_buffer) {
	return 0;
}

JNIEXPORT jboolean JNICALL Java_org_lwjgl_opengl_LinuxEvent_nFilterEvent(JNIEnv *env, jclass unused, jobject event_buffer, jlong window_ptr) {
	return JNI_FALSE;
}

JNIEXPORT void JNICALL Java_org_lwjgl_opengl_LinuxEvent_nNextEvent(JNIEnv *env, jclass unused, jlong display_ptr, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	memcpy(mapped_event, &event, sizeof(SDL_Event));
}

static jint map_window_event_type(SDL_WindowEvent *e) {
	switch (e->event) {
		case SDL_WINDOWEVENT_ENTER:
			return 7;
		case SDL_WINDOWEVENT_LEAVE:
			return 8;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			return 9;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			return 10;
		case SDL_WINDOWEVENT_RESIZED:
			return 22;
		case SDL_WINDOWEVENT_CLOSE:
			return 33;
		default:
			return 12;
	}
}

static jint map_event_type(SDL_Event *e) {
	switch (e->type) {
		case SDL_WINDOWEVENT:
			return map_window_event_type(&e->window);
		case SDL_KEYDOWN:
			keysym = e->key.keysym;
			return 2;
		case SDL_KEYUP:
			keysym = e->key.keysym;
			return 3;
		case SDL_MOUSEMOTION:
			return 6;
		case SDL_MOUSEBUTTONDOWN:
			return 4;
		case SDL_MOUSEWHEEL:
		case SDL_MOUSEBUTTONUP:
			return 5;
		default:
			return 12;
	}
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetType(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return map_event_type(mapped_event);
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetWindow(JNIEnv *env, jclass unused, jobject event_buffer) {
	return (intptr_t)context_window;
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetClientMessageType(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	if (mapped_event->type == SDL_MOUSEMOTION) {
		return 2;
	}
	return 0;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetClientData(JNIEnv *env, jclass unused, jobject event_buffer, jint index) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	if (mapped_event->type == SDL_MOUSEMOTION) {
		switch (index) {
			case 0:
				return mapped_event->motion.x;
			case 1:
			default:
				return mapped_event->motion.y;
		}
	}
	if (mapped_event->type == SDL_WINDOWEVENT) {
		if (mapped_event->window.event == SDL_WINDOWEVENT_CLOSE) {
			return 1;
		}
	}

	// NOTHING
	return 0;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetClientFormat(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	if (mapped_event->type == SDL_WINDOWEVENT) {
		if (mapped_event->window.event == SDL_WINDOWEVENT_CLOSE) {
			return 32;
		}
	}
	return 0;
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonTime(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return mapped_event->button.timestamp;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonState(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return mapped_event->button.state;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonType(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	if (mapped_event->type == SDL_MOUSEWHEEL) {
		// ah yes this is indeed a mouse press
		return 4;
	}
	switch (mapped_event->button.type) {
		case SDL_MOUSEBUTTONDOWN:
			return 4;
		case SDL_MOUSEBUTTONUP:
			return 5;
		default:
			return 0;
	}
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonButton(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	if (mapped_event->type == SDL_MOUSEWHEEL) {
		SDL_MouseWheelEvent e = mapped_event->wheel;
		switch (e.y) {
			case -1:
				return 5;
			case 1:
				return 4;
		}
	}
	return mapped_event->button.button;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonX(JNIEnv *env, jclass unused, jobject event_buffer) {
	static int x = 0;
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	// this usually gets called during a menu
	// so we just reset the internal camera x and y
	if (!SDL_GetRelativeMouseMode()) {
		x = 0;
		return mapped_event->motion.x;
	}
	x += mapped_event->motion.xrel;
	return x;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonY(JNIEnv *env, jclass unused, jobject event_buffer) {
	static int y = 0;
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	// this usually gets called during a menu
	// so we just reset the internal camera x and y
	if (!SDL_GetRelativeMouseMode()) {
		y = 0;
		return mapped_event->motion.y;
	}
	y += mapped_event->motion.yrel;
	return y;
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetKeyAddress(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return (intptr_t)mapped_event;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetKeyTime(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return mapped_event->key.timestamp;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetKeyType(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	switch (mapped_event->key.type) {
		case SDL_KEYDOWN:
			return 2;
		case SDL_KEYUP:
			return 3;
		default:
			return 0;
	}
}

static jint convert_function_keycodes(SDL_Keycode sym) {
	if (sym < SDLK_F1 || sym > SDLK_F12) {
		return -1;
	}
	int i = SDLK_F12 - sym;
	// XK_F12
	// - the numerical difference between SDLK_F12 and sym
	// which could range between 0 and 12
	return 0xffc9 - i;
}

jint convert_keycode(SDL_Keycode sym) {
	jint keycode = convert_function_keycodes(sym);
	if (keycode > 0) {
		return keycode;
	}

	switch (sym) {
		case SDLK_BACKSPACE:
		case SDLK_TAB:
		case SDLK_RETURN:
		case SDLK_ESCAPE:
			return sym | 0xff00; // the upper four are the same as X11's keys when converted via or
		case SDLK_LSHIFT:
			return 0xffe1; // XK_Shift_L
		case SDLK_RSHIFT:
			return 0xffe2; // XK_Shift_R
		case SDLK_LCTRL:
			return 0xffe3; // XK_Control_L
		case SDLK_RCTRL:
			return 0xffe4; // XK_Control_R
		default:
			return sym;
	}
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetKeyKeyCode(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	SDL_Keycode sym = mapped_event->key.keysym.sym;
	return convert_keycode(sym);
}

// ???????? what is this function supposed to do?
// it seems like it is redundant.
// however I can't remove it :(
JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetKeyState(JNIEnv *env, jclass unused, jobject event_buffer) {
	SDL_Event *mapped_event = (SDL_Event *)(*env)->GetDirectBufferAddress(env, event_buffer);
	SDL_Keycode sym = mapped_event->key.keysym.sym;
	return convert_keycode(sym);
}
