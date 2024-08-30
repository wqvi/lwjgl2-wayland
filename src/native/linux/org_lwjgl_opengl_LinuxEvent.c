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

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <jni.h>
#include "common_tools.h"
#include "org_lwjgl_opengl_LinuxEvent.h"
#define NO_SDL_GLEXT
#include <SDL2/SDL.h>

extern int is_running;
extern SDL_Window *context_window;
static SDL_Event event;

JNIEXPORT jobject JNICALL Java_org_lwjgl_opengl_LinuxEvent_createEventBuffer(JNIEnv *env, jclass unused) {
	return newJavaManagedByteBuffer(env, sizeof(XEvent));
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_getPending(JNIEnv *env, jclass unused, jlong display_ptr) {
	return SDL_PollEvent(&event);
}

JNIEXPORT void JNICALL Java_org_lwjgl_opengl_LinuxEvent_nSetWindow(JNIEnv *env, jclass unused, jobject event_buffer, jlong window_ptr) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	Window window = (Window)window_ptr;
	event->xany.window = window;
}

JNIEXPORT void JNICALL Java_org_lwjgl_opengl_LinuxEvent_nSendEvent(JNIEnv *env, jclass unused, jobject event_buffer, jlong display_ptr, jlong window_ptr, jboolean propagate, jlong eventmask) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	Display *disp = (Display *)(intptr_t)display_ptr;
	Window window = (Window)window_ptr;
	XSendEvent(disp, window, propagate == JNI_TRUE ? True : False, eventmask, event);
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetFocusDetail(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xfocus.detail;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetFocusMode(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xfocus.mode;
}

JNIEXPORT jboolean JNICALL Java_org_lwjgl_opengl_LinuxEvent_nFilterEvent(JNIEnv *env, jclass unused, jobject event_buffer, jlong window_ptr) {
	XEvent *mapped_event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	if (mapped_event->type == 4 || mapped_event->type == 5) {
		return JNI_TRUE;
	} else if (mapped_event->type == 6) {
		return JNI_FALSE;
	}
	return JNI_FALSE;
}

static void window_event(XEvent **mapped_event) {
	switch (event.window.event) {
		case SDL_WINDOWEVENT_CLOSE:
			(*mapped_event)->type = 33;
			return;
	}
}

static void motion_event(XEvent **mapped_event) {
	(*mapped_event)->type = 6;
	(*mapped_event)->xbutton.time = event.motion.timestamp;
	(*mapped_event)->xbutton.x = event.motion.x;
	(*mapped_event)->xbutton.y = event.motion.y;
}

JNIEXPORT void JNICALL Java_org_lwjgl_opengl_LinuxEvent_nNextEvent(JNIEnv *env, jclass unused, jlong display_ptr, jobject event_buffer) {
	XEvent *mapped_event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	switch (event.type) {
		case SDL_WINDOWEVENT:
			window_event(&mapped_event);
			break;
		case SDL_MOUSEMOTION:
			motion_event(&mapped_event);
			break;
		default:
			mapped_event->type = 12;
			break;

	}
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetType(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *mapped_event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return mapped_event->type;
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetWindow(JNIEnv *env, jclass unused, jobject event_buffer) {
	return (intptr_t)context_window;
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetClientMessageType(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xclient.message_type;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetClientData(JNIEnv *env, jclass unused, jobject event_buffer, jint index) {
	if (index == 0) {
		// WM_DELETE_WINDOW
		return 1;
	}
	// NOTHING
	return 0;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetClientFormat(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *mapped_event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	if (mapped_event->type == 33) {
		// CLOSE WINDOW
		return 32;
	}
	return 0;
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonTime(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *mapped_event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return mapped_event->xbutton.time;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonState(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xbutton.state;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonType(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xbutton.type;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonButton(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xbutton.button;
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonRoot(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return (intptr_t)context_window;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonXRoot(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xbutton.x;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonYRoot(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xbutton.y;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonX(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xbutton.x;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetButtonY(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xbutton.y;
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetKeyAddress(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	XKeyEvent *key_event = &(event->xkey);
	return (jlong)(intptr_t)key_event;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetKeyTime(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xkey.time;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetKeyType(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xkey.type;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetKeyKeyCode(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xkey.keycode;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxEvent_nGetKeyState(JNIEnv *env, jclass unused, jobject event_buffer) {
	XEvent *event = (XEvent *)(*env)->GetDirectBufferAddress(env, event_buffer);
	return event->xkey.state;
}
