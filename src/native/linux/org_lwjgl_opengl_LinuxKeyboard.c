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
 * Linux keyboard handling.
 *
 * @author elias_naur <elias_naur@users.sourceforge.net>
 * @version $Revision: 2399 $
 */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include "common_tools.h"
#include "org_lwjgl_opengl_LinuxKeyboard.h"
#define NO_SDL_GLEXT
#include <SDL2/SDL.h>

// TODO what is this? A global variable?
// Wow I was hacky
extern SDL_Keysym keysym;

extern jint convert_keycode(SDL_Keycode sym);

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxKeyboard_getMaxKeyPerMod(JNIEnv *env, jclass unsused, jlong mapping_ptr) {
	return 1;
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxKeyboard_lookupModifierMap(JNIEnv *env, jclass unused, jlong mapping_ptr, jint index) {
	return keysym.sym;
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxKeyboard_lookupKeysym(JNIEnv *env, jclass unused, jlong event_ptr, jint index) {
	return convert_keycode(keysym.sym);
}

JNIEXPORT jlong JNICALL Java_org_lwjgl_opengl_LinuxKeyboard_toUpper(JNIEnv *env, jclass unused, jlong keysym) {
	return 0;
}

static char handle_shift_modifier(SDL_Keycode sym) {
	// this is scuffed but SDL_GetKeyName doesn't exactly
	// work the way I'd want it to.
	// least this way the compiler *might* optimize this to a
	// lookup table for characters
	
	// lets hope it's an optimizing compiler :)
	switch (sym) {
		case SDLK_1:
			return '!';
		case SDLK_2:
			return '@';
		case SDLK_3:
			return '#';
		case SDLK_4:
			return '$';
		case SDLK_5:
			return '%';
		case SDLK_6:
			return '^';
		case SDLK_7:
			return '&';
		case SDLK_8:
			return '*';
		case SDLK_9:
			return '(';
		case SDLK_0:
			return ')';
		case SDLK_MINUS:
			return '_';
		case SDLK_EQUALS:
			return '+';
		case SDLK_SEMICOLON:
			return ':';
		case SDLK_QUOTE:
			return '"';
		case SDLK_COMMA:
			return '<';
		case SDLK_PERIOD:
			return '>';
		case SDLK_SLASH:
			return '?';
		case SDLK_LEFTBRACKET:
			return '{';
		case SDLK_RIGHTBRACKET:
			return '}';
		case SDLK_BACKSLASH:
			return '|';
		case SDLK_BACKQUOTE:
			return '~';
		default:
			return toupper((char)sym);
	}
}

JNIEXPORT jint JNICALL Java_org_lwjgl_opengl_LinuxKeyboard_lookupString(JNIEnv *env, jclass unused, jlong event_ptr, jobject buffer_obj) {
	SDL_Event *event = (SDL_Event *)event_ptr;
	char *buffer = (char *)(*env)->GetDirectBufferAddress(env, buffer_obj);

	if (event->type & (SDL_KEYDOWN | SDL_KEYUP)) {
		SDL_Keysym keysym = event->key.keysym;
		SDL_Keycode sym = keysym.sym;
		Uint16 mod = keysym.mod;
		if (sym >= 32 && sym <= 126) {
			char c = (char)sym;
			if (mod & KMOD_SHIFT) {
				c = handle_shift_modifier(c);
			}
			buffer[0] = c;
			return 1;
		}
	}

	return 0;
}
