/*
 * Copyright (c) 2002-2010 LWJGL Project
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
package org.lwjgl.opengl;

/**
 * This is the Display implementation interface. Display delegates
 * to implementors of this interface. There is one DisplayImplementation
 * for each supported platform.
 * @author elias_naur
 */

import java.awt.Canvas;
import java.awt.event.FocusListener;
import java.awt.event.FocusEvent;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.ByteOrder;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.lang.reflect.InvocationTargetException;

import org.lwjgl.BufferUtils;
import org.lwjgl.LWJGLException;
import org.lwjgl.LWJGLUtil;
import org.lwjgl.MemoryUtil;
import org.lwjgl.opengl.XRandR.Screen;
import org.lwjgl.opengles.EGL;

import java.security.AccessController;
import java.security.PrivilegedAction;
import java.util.ArrayList;
import java.util.List;

final class LinuxDisplay implements DisplayImplementation {
	/* X11 constants */
	public static final int CurrentTime = 0;
	public static final int GrabSuccess = 0;
	public static final int AutoRepeatModeOff  = 0;
	public static final int AutoRepeatModeOn = 1;
	public static final int AutoRepeatModeDefault = 2;
	public static final int None = 0;

	private static final int KeyPressMask = 1 << 0;
	private static final int KeyReleaseMask = 1 << 1;
	private static final int ButtonPressMask = 1 << 2;
	private static final int ButtonReleaseMask = 1 << 3;

	private static final int NotifyAncestor = 0;
	private static final int NotifyNonlinear = 3;
	private static final int NotifyPointer = 5;
	private static final int NotifyPointerRoot = 6;
	private static final int NotifyDetailNone = 7;

	private static final int SetModeInsert = 0;
	private static final int SaveSetRoot = 1;
	private static final int SaveSetUnmap = 1;
	
	private static final int X_SetInputFocus = 42;

	/** Window mode enum */
	private static final int FULLSCREEN_LEGACY = 1;
	private static final int WINDOWED = 0;

	/** Current window mode */
	private static boolean fullscreen = false;

	private static final int XF86VIDMODE = 11;

	/** Current X11 Display pointer */
	private static long display;
	private static long current_window;
	private static long saved_error_handler;

	private static int display_connection_usage_count;

	/** Event buffer */
	private final LinuxEvent event_buffer = new LinuxEvent();
	private final LinuxEvent tmp_event_buffer = new LinuxEvent();

	/** Atom used for the pointer warp messages */
	private long delete_atom;

	private PeerInfo peer_info;

	/** Saved mode to restore with */
	private DisplayMode saved_mode;

	private boolean keyboard_grabbed;
	private boolean pointer_grabbed;
	private boolean input_released;
	private boolean grab;
	private boolean focused;
	private boolean minimized;
	private boolean dirty;
	private boolean close_requested;
	private long current_cursor;
	private long blank_cursor;
	private boolean mouseInside = true;
	private boolean resizable;
	private boolean resized;
	
	private int window_x;
	private int window_y;
	private int window_width;
	private int window_height;
	
	private Canvas parent;
	private long parent_window;
	private static boolean xembedded;
	private long parent_proxy_focus_window;
	private boolean parent_focused;
	private boolean parent_focus_changed;
	private long last_window_focus = 0;

	private LinuxKeyboard keyboard;
	private LinuxMouse mouse;
	
	private String wm_class;

	private final FocusListener focus_listener = new FocusListener() {
		public void focusGained(FocusEvent e) {
			synchronized (GlobalLock.lock) {
				parent_focused = true;
				parent_focus_changed = true;
			}
		}
		public void focusLost(FocusEvent e) {
			synchronized (GlobalLock.lock) {
				parent_focused = false;
				parent_focus_changed = true;
			}
		}
	};

	static void lockAWT() {
	}

	static void unlockAWT() {
	}

	/**
	 * increment and decrement display usage.
	 */
	static void incDisplay() throws LWJGLException {
		if (display_connection_usage_count == 0) {
			try {
				// TODO: Can we know if we're on desktop or ES?
				GLContext.loadOpenGLLibrary();
				org.lwjgl.opengles.GLContext.loadOpenGLLibrary();
			} catch (Throwable t) {
			}
			saved_error_handler = setErrorHandler();
			display = openDisplay();
//			synchronize(display, true);
		}
		display_connection_usage_count++;
	}
	private static native int callErrorHandler(long handler, long display, long error_ptr);
	private static native long setErrorHandler();
	private static native long resetErrorHandler(long handler);
	private static native void synchronize(long display, boolean synchronize);

	private static int globalErrorHandler(long display, long event_ptr, long error_display, long serial, long error_code, long request_code, long minor_code) throws LWJGLException {
		if (xembedded && request_code == X_SetInputFocus) return 0; // ignore X error in xembeded mode to fix a browser issue when dragging or switching tabs 
		
		if (display == getDisplay()) {
			String error_msg = getErrorText(display, error_code);
			throw new LWJGLException("X Error - disp: 0x" + Long.toHexString(error_display) + " serial: " + serial + " error: " + error_msg + " request_code: " + request_code + " minor_code: " + minor_code);
		} else if (saved_error_handler != 0)
			return callErrorHandler(saved_error_handler, display, event_ptr);
		return 0;
	}
	private static native String getErrorText(long display, long error_code);

	static void decDisplay() {
		/*
		 * Some drivers (at least some versions of the radeon dri driver)
		 * don't like it when the display is closed and later re-opened,
		 * so we'll just let the singleton display connection leak.
		 */
/*		display_connection_usage_count--;
		if (display_connection_usage_count < 0)
			throw new InternalError("display_connection_usage_count < 0: " + display_connection_usage_count);
		if (display_connection_usage_count == 0) {
			closeDisplay(display);
			resetErrorHandler(saved_error_handler);
			display = 0;
			GLContext.unloadOpenGLLibrary();
		}*/
	}

	static native long openDisplay() throws LWJGLException;
	static native void closeDisplay(long display);

	static long getDisplay() {
		if (display_connection_usage_count <= 0)
			throw new InternalError("display_connection_usage_count = " + display_connection_usage_count);
		return display;
	}

	static int getDefaultScreen() {
		return nGetDefaultScreen(getDisplay());
	}
	static native int nGetDefaultScreen(long display);

	static long getWindow() {
		return current_window;
	}

	private void ungrabKeyboard() {
		if (keyboard_grabbed) {
			nUngrabKeyboard();
			keyboard_grabbed = false;
		}
	}
	static native void nUngrabKeyboard();

	private void grabKeyboard() {
		if (!keyboard_grabbed) {
			nGrabKeyboard();
			keyboard_grabbed = true;
		}
	}
	static native void nGrabKeyboard();

	private void grabPointer() {
		if (!pointer_grabbed) {
			int result = nGrabPointer();
			if (result == GrabSuccess) {
				pointer_grabbed = true;
				// make sure we have a centered window
				if (fullscreen) {
					nSetViewPort(getDisplay(), getWindow(), getDefaultScreen());
				}
			}
		}
	}
	static native int nGrabPointer();
	private static native void nSetViewPort(long display, long window, int screen);

	private void ungrabPointer() {
		if (pointer_grabbed) {
			pointer_grabbed = false;
			nUngrabPointer();
		}
	}
	static native int nUngrabPointer();

	private static boolean isFullscreen() {
		return fullscreen;
	}

	private boolean shouldGrab() {
		return !input_released && grab && mouse != null;
	}

	private void updatePointerGrab() {
		if (shouldGrab()) {
			grabPointer();
		} else {
			ungrabPointer();
		}
		updateCursor();
	}

	private void updateCursor() {
		long cursor;
		if (shouldGrab()) {
			cursor = blank_cursor;
		} else {
			cursor = current_cursor;
		}
		nDefineCursor(getDisplay(), getWindow(), cursor);
	}
	private static native void nDefineCursor(long display, long window, long cursor_handle);

	private void updateKeyboardGrab() {
		if (fullscreen)
			grabKeyboard();
		else
			ungrabKeyboard();
	}

	public void createWindow(final DrawableLWJGL drawable, DisplayMode mode, Canvas parent, int x, int y) throws LWJGLException {
		window_x = x;
		window_y = y;
		window_width = mode.getWidth();
		window_height = mode.getHeight();
		fullscreen = mode.isFullscreenCapable();
		current_window = nCreateWindow(fullscreen, x, y, window_width, window_height);
	}

	private static native long nCreateWindow(boolean fullscreen, int x, int y, int width, int height) throws LWJGLException;
	private static native long nGetInputFocus(long display) throws LWJGLException;
	private static native void nSetInputFocus(long display, long window, long time);
	private static native void nSetWindowSize(long display, long window, int width, int height, boolean resizable);
	private static native int nGetX();
	private static native int nGetY();
	private static native int nGetWidth();
	private static native int nGetHeight();

	private static long getHandle(Canvas parent) throws LWJGLException {
		AWTCanvasImplementation awt_impl = AWTGLCanvas.createImplementation();
		LinuxPeerInfo parent_peer_info = (LinuxPeerInfo)awt_impl.createPeerInfo(parent, null, null);
		ByteBuffer parent_peer_info_handle = parent_peer_info.lockAndGetHandle();
		try {
			return parent_peer_info.getDrawable();
		} finally {
			parent_peer_info.unlock();
		}
	}

	private void updateInputGrab() {
		updatePointerGrab();
		updateKeyboardGrab();
	}

	public void destroyWindow() {
		try {
			if (parent != null) {
				parent.removeFocusListener(focus_listener);
			}
			try {
				setNativeCursor(null);
			} catch (LWJGLException e) {
				LWJGLUtil.log("Failed to reset cursor: " + e.getMessage());
			}
			nDestroyCursor(getDisplay(), blank_cursor);
			blank_cursor = None;
			ungrabKeyboard();
			nDestroyWindow(getDisplay(), getWindow());
			decDisplay();

			if (!fullscreen)
				Compiz.setLegacyFullscreenSupport(false);
		} finally {
		}
	}
	static native void nDestroyWindow(long display, long window);

	public void switchDisplayMode(DisplayMode mode) throws LWJGLException {
	}
	private static native void nSwitchDisplayMode(boolean fullscreen) throws LWJGLException;

	private static long internAtom(String atom_name, boolean only_if_exists) throws LWJGLException {
		incDisplay();
		try {
			return nInternAtom(getDisplay(), atom_name, only_if_exists);
		} finally {
			decDisplay();
		}
	}
	static native long nInternAtom(long display, String atom_name, boolean only_if_exists);

	public void resetDisplayMode() {
		try {
			nSwitchDisplayMode(fullscreen);
		} catch (LWJGLException e) {
			e.printStackTrace();
		}

		Compiz.setLegacyFullscreenSupport(false);
	}

	public int getGammaRampLength() {
		return 0;
	}

	public void setGammaRamp(FloatBuffer gammaRamp) throws LWJGLException {
	}

	public String getAdapter() {
		return null;
	}

	public String getVersion() {
		return null;
	}

	public DisplayMode init() throws LWJGLException {
		Compiz.init();
		delete_atom = internAtom("WM_DELETE_WINDOW", false);
		DisplayMode[] modes = getAvailableDisplayModes();
		saved_mode = modes[0];	
		fullscreen = saved_mode.isFullscreenCapable();
		return saved_mode;
	}

	public void setTitle(String title) {
		final ByteBuffer titleText = MemoryUtil.encodeUTF8(title);
		nSetTitle(MemoryUtil.getAddress(titleText));
	}
	private static native void nSetTitle(long title);
	
	public boolean isCloseRequested() {
		boolean result = close_requested;
		close_requested = false;
		return result;
	}

	public boolean isVisible() {
		return !minimized;
	}

	public boolean isActive() {
		return focused || fullscreen;
	}

	public boolean isDirty() {
		boolean result = dirty;
		dirty = false;
		return result;
	}

	public PeerInfo createPeerInfo(PixelFormat pixel_format, ContextAttribs attribs) throws LWJGLException {
		peer_info = new LinuxDisplayPeerInfo(pixel_format);
		return peer_info;
	}

	private void processEvents() {
		while (LinuxEvent.getPending(getDisplay()) > 0) {
			event_buffer.nextEvent(getDisplay());
			long event_window = event_buffer.getWindow();
			if (event_window != getWindow() || event_buffer.filterEvent(event_window) ||
					(mouse != null && mouse.filterEvent(grab, shouldWarpPointer(), event_buffer)) ||
					 (keyboard != null && keyboard.filterEvent(event_buffer)))
				continue;
			switch (event_buffer.getType()) {
				case LinuxEvent.FocusIn:
					setFocused(true, event_buffer.getFocusDetail());
					break;
				case LinuxEvent.FocusOut:
					setFocused(false, event_buffer.getFocusDetail());
					break;
				case LinuxEvent.ClientMessage:
					if ((event_buffer.getClientFormat() == 32) && (event_buffer.getClientData(0) == delete_atom))
						close_requested = true;
					break;
				case LinuxEvent.MapNotify:
					dirty = true;
					minimized = false;
					break;
				case LinuxEvent.UnmapNotify:
					dirty = true;
					minimized = true;
					break;
				case LinuxEvent.Expose:
					dirty = true;
					break;
				case LinuxEvent.ConfigureNotify:
					int x = nGetX();
					int y = nGetY();
					
					int width = nGetWidth();
					int height = nGetHeight();
					
					window_x = x;
					window_y = y;
					
					if (window_width != width || window_height != height) {
						resized = true;
						window_width = width;
						window_height = height;
					}
					
					break;
				case LinuxEvent.EnterNotify:
					mouseInside = true;
					break;
				case LinuxEvent.LeaveNotify:
					mouseInside = false;
					break;
				default:
					break;
			}
		}
	}

	public void update() {
		processEvents();
		checkInput();
	}

	public void reshape(int x, int y, int width, int height) {
		nReshape(getDisplay(), getWindow(), x, y, width, height);
	}
	private static native void nReshape(long display, long window, int x, int y, int width, int height);

	public DisplayMode[] getAvailableDisplayModes() throws LWJGLException {
                return nGetAvailableDisplayModes(getDisplay(), getDefaultScreen(), XF86VIDMODE);
	}
	private static native DisplayMode[] nGetAvailableDisplayModes(long display, int screen, int extension) throws LWJGLException;

	/* Mouse */
	public boolean hasWheel() {
		return true;
	}

	public int getButtonCount() {
		return mouse.getButtonCount();
	}

	public void createMouse() throws LWJGLException {
		try {
			mouse = new LinuxMouse(getDisplay(), getWindow(), getWindow());
		} finally {
		}
	}

	public void destroyMouse() {
		mouse = null;
		updateInputGrab();
	}

	public void pollMouse(IntBuffer coord_buffer, ByteBuffer buttons) {
		try {
			mouse.poll(grab, coord_buffer, buttons);
		} finally {
		}
	}

	public void readMouse(ByteBuffer buffer) {
		try {
			mouse.read(buffer);
		} finally {
		}
	}

	public void setCursorPosition(int x, int y) {
		try {
			mouse.setCursorPosition(x, y);
		} finally {
		}
	}

	private void checkInput() {
		if (parent == null) return;

		if (xembedded) {
			long current_focus_window = 0;

			if (last_window_focus != current_focus_window || parent_focused != focused) {
				if (isParentWindowActive(current_focus_window)) {
					if (parent_focused) {
						nSetInputFocus(getDisplay(), current_window, CurrentTime);
						last_window_focus = current_window;
						focused = true;
					}
					else {
						// return focus to the parent proxy focus window
						nSetInputFocus(getDisplay(), parent_proxy_focus_window, CurrentTime);
						last_window_focus = parent_proxy_focus_window;
						focused = false;
					}
				}
				else {
					last_window_focus = current_focus_window;
					focused = false;
				}
			}
		}
		else {
			if (parent_focus_changed && parent_focused) {
				setInputFocusUnsafe(getWindow());
				parent_focus_changed = false;
			}
		}
	}
	
	private void setInputFocusUnsafe(long window) {
		try {
			nSetInputFocus(getDisplay(), window, CurrentTime);
			nSync(getDisplay(), false);
		} catch (LWJGLException e) {
			// Since we don't have any event timings for XSetInputFocus, a race condition might give a BadMatch, which we'll catch and ignore
			LWJGLUtil.log("Got exception while trying to focus: " + e);
		}
	}
	
	private static native void nSync(long display, boolean throw_away_events) throws LWJGLException;

	/**
	 * This method will check if the parent window is active when running
	 * in xembed mode. Every xembed embedder window has a focus proxy
	 * window that recieves all the input. This method will test whether
	 * the provided window handle is the focus proxy, if so it will get its
	 * parent window and then test whether this is an ancestor to our
	 * current_window. If so then parent window is active.
	 *
	 * @param window - the window handle to test
	 */
	private boolean isParentWindowActive(long window) {
		return false; // failed to find an active parent window
	}

	private void setFocused(boolean got_focus, int focus_detail) {
		if (focused == got_focus || focus_detail == NotifyDetailNone || focus_detail == NotifyPointer || focus_detail == NotifyPointerRoot || xembedded)
			return;
		focused = got_focus;

		if (focused) {
			acquireInput();
		}
		else {
			releaseInput();
		}
	}

	private void releaseInput() {
		if (fullscreen || input_released)
			return;
		if ( keyboard != null )
			keyboard.releaseAll();
		input_released = true;
		updateInputGrab();
	}
	private static native void nIconifyWindow(long display, long window, int screen);

	private void acquireInput() {
		if (fullscreen || !input_released)
			return;
		input_released = false;
		updateInputGrab();
	}

	public void grabMouse(boolean new_grab) {
		try {
			if (new_grab != grab) {
				grab = new_grab;
				updateInputGrab();
				mouse.changeGrabbed(grab, shouldWarpPointer());
			}
		} finally {
		}
	}

	private boolean shouldWarpPointer() {
		return pointer_grabbed && shouldGrab();
	}

	public int getNativeCursorCapabilities() {
		try {
			incDisplay();
			try {
				return nGetNativeCursorCapabilities(getDisplay());
			} finally {
				decDisplay();
			}
		} catch (LWJGLException e) {
			throw new RuntimeException(e);
		} finally {
		}
	}
	private static native int nGetNativeCursorCapabilities(long display) throws LWJGLException;

	public void setNativeCursor(Object handle) throws LWJGLException {
		current_cursor = getCursorHandle(handle);
		try {
			updateCursor();
		} finally {
		}
	}

	public int getMinCursorSize() {
		try {
			incDisplay();
			try {
				return nGetMinCursorSize(getDisplay(), getWindow());
			} finally {
				decDisplay();
			}
		} catch (LWJGLException e) {
			LWJGLUtil.log("Exception occurred in getMinCursorSize: " + e);
			return 0;
		} finally {
		}
	}
	private static native int nGetMinCursorSize(long display, long window);

	public int getMaxCursorSize() {
		try {
			incDisplay();
			try {
				return nGetMaxCursorSize(getDisplay(), getWindow());
			} finally {
				decDisplay();
			}
		} catch (LWJGLException e) {
			LWJGLUtil.log("Exception occurred in getMaxCursorSize: " + e);
			return 0;
		} finally {
		}
	}
	private static native int nGetMaxCursorSize(long display, long window);

	/* Keyboard */
	public void createKeyboard() throws LWJGLException {
		try {
			keyboard = new LinuxKeyboard(getDisplay(), getWindow());
		} finally {
		}
	}

	public void destroyKeyboard() {
		try {
			keyboard.destroy(getDisplay());
			keyboard = null;
		} finally {
		}
	}

	public void pollKeyboard(ByteBuffer keyDownBuffer) {
		try {
			keyboard.poll(keyDownBuffer);
		} finally {
		}
	}

	public void readKeyboard(ByteBuffer buffer) {
		try {
			keyboard.read(buffer);
		} finally {
		}
	}

	private static native long nCreateCursor(long display, int width, int height, int xHotspot, int yHotspot, int numImages, IntBuffer images, int images_offset, IntBuffer delays, int delays_offset) throws LWJGLException;

	private static long createBlankCursor() {
		return nCreateBlankCursor(getDisplay(), getWindow());
	}
	static native long nCreateBlankCursor(long display, long window);

	public Object createCursor(int width, int height, int xHotspot, int yHotspot, int numImages, IntBuffer images, IntBuffer delays) throws LWJGLException {
		try {
			incDisplay();
			try {
				long cursor = nCreateCursor(getDisplay(), width, height, xHotspot, yHotspot, numImages, images, images.position(), delays, delays != null ? delays.position() : -1);
				return cursor;
			} catch (LWJGLException e) {
				decDisplay();
				throw e;
			}
		} finally {
		}
	}

	private static long getCursorHandle(Object cursor_handle) {
		return cursor_handle != null ? (Long)cursor_handle : None;
	}

	public void destroyCursor(Object cursorHandle) {
		try {
			nDestroyCursor(getDisplay(), getCursorHandle(cursorHandle));
			decDisplay();
		} finally {
		}
	}
	static native void nDestroyCursor(long display, long cursorHandle);

	public int getPbufferCapabilities() {
		try {
			incDisplay();
			try {
				return nGetPbufferCapabilities(getDisplay(), getDefaultScreen());
			} finally {
				decDisplay();
			}
		} catch (LWJGLException e) {
			LWJGLUtil.log("Exception occurred in getPbufferCapabilities: " + e);
			return 0;
		} finally {
		}
	}
	private static native int nGetPbufferCapabilities(long display, int screen);

	public boolean isBufferLost(PeerInfo handle) {
		return false;
	}

	public PeerInfo createPbuffer(int width, int height, PixelFormat pixel_format, ContextAttribs attribs,
			IntBuffer pixelFormatCaps,
			IntBuffer pBufferAttribs) throws LWJGLException {
		return new LinuxPbufferPeerInfo(width, height, pixel_format);
	}

	public void setPbufferAttrib(PeerInfo handle, int attrib, int value) {
		throw new UnsupportedOperationException();
	}

	public void bindTexImageToPbuffer(PeerInfo handle, int buffer) {
		throw new UnsupportedOperationException();
	}

	public void releaseTexImageFromPbuffer(PeerInfo handle, int buffer) {
		throw new UnsupportedOperationException();
	}
	
	/**
	 * This method will convert icon bytebuffers into a single bytebuffer
	 * as the icon format required by _NET_WM_ICON should be in a cardinal
	 * 32 bit ARGB format i.e. all icons in a single buffer the data starting
	 * with 32 bit width & height followed by the color data as 32bit ARGB.
	 * 
	 * @param icons Array of icons in RGBA format
	 */
	private static ByteBuffer convertIcons(ByteBuffer[] icons) {
		
		int bufferSize = 0;
		
		// calculate size of bytebuffer
		for ( ByteBuffer icon : icons ) {
			int size = icon.limit() / 4;
			int dimension = (int)Math.sqrt(size);
			if ( dimension > 0 ) {
				bufferSize += 2 * 4; // add 32 bit width & height, 4 bytes each
				bufferSize += dimension * dimension * 4;
			}
		}
		
		if (bufferSize == 0) return null;
		
		ByteBuffer icon_argb = BufferUtils.createByteBuffer(bufferSize);//icon.capacity()+(2*4));
		icon_argb.order(ByteOrder.BIG_ENDIAN);
		
		for ( ByteBuffer icon : icons ) {
			int size = icon.limit() / 4;
			int dimension = (int)Math.sqrt(size);
			
			icon_argb.putInt(dimension); // width
			icon_argb.putInt(dimension); // height
			
			for (int y = 0; y < dimension; y++) {
				for (int x = 0; x < dimension; x++) {
					
					byte r = icon.get((x*4)+(y*dimension*4));
					byte g = icon.get((x*4)+(y*dimension*4)+1);
					byte b = icon.get((x*4)+(y*dimension*4)+2);
					byte a = icon.get((x*4)+(y*dimension*4)+3);
					
					icon_argb.put(a);
					icon_argb.put(r);
					icon_argb.put(g);
					icon_argb.put(b);
				}
			}
		}
		
		return icon_argb;
	}

	/**
	 * Sets one or more icons for the Display.
	 * <ul>
	 * <li>On Windows you should supply at least one 16x16 icon and one 32x32.</li>
	 * <li>Linux (and similar platforms) expect one 32x32 icon.</li>
	 * <li>Mac OS X should be supplied one 128x128 icon</li>
	 * </ul>
	 * The implementation will use the supplied ByteBuffers with image data in RGBA and perform any conversions necessary for the specific platform.
	 *
	 * @param icons Array of icons in RGBA mode
	 * @return number of icons used.
	 */
	public int setIcon(ByteBuffer[] icons) {
		try {
			incDisplay();
			try {
				// get icons as cardinal ARGB format
				ByteBuffer icons_data = convertIcons(icons);
				if (icons_data == null) return 0;
				nSetWindowIcon(getDisplay(), getWindow(), icons_data, icons_data.capacity());//, icon_mask, icon_mask.capacity(), dimension, dimension);
				return icons.length;
			} finally {
				decDisplay();
			}
		} catch (LWJGLException e) {
			LWJGLUtil.log("Failed to set display icon: " + e);
			return 0;
		} finally {
		}
	}

	private static native void nSetWindowIcon(long display, long window, ByteBuffer icons_data, int icons_size);

	public int getX() {
		return window_x;
	}

	public int getY() {
		return window_y;
	}
	
	public int getWidth() {
		return window_width;
	}

	public int getHeight() {
		return window_height;
	}

	public boolean isInsideWindow() {
		return mouseInside;
	}

	public void setResizable(boolean resizable) {
		if (this.resizable == resizable) {
			return;
		}
		
		this.resizable = resizable;
		nSetWindowSize(getDisplay(), getWindow(), window_width, window_height, resizable);
	}

	public boolean wasResized() {
		if (resized) {
			resized = false;
			return true;
		}
		
		return false;
	}

	public float getPixelScaleFactor() {
		return 1f;
	}
	
	/**
	 * Helper class for managing Compiz's workarounds. We need this to enable Legacy
	 * Fullscreen Support in Compiz, else we'll have trouble with fullscreen windows
	 * when Compiz effects are enabled.
	 *
	 * Implementation Note: This code is probably too much for an inner class, but
	 * keeping it here until we're sure we cannot find a better solution.
	 */
	private static final class Compiz {

		private static boolean applyFix;

		private static Provider provider;

		private Compiz() {
		}

		static void init() {
			if ( Display.getPrivilegedBoolean("org.lwjgl.opengl.Window.nocompiz_lfs") )
				return;

			AccessController.doPrivileged(new PrivilegedAction<Object>() {
				public Object run() {
					try {
						// Check if Compiz is active
						if ( !isProcessActive("compiz") )
							return null;

						provider = null;

						String providerName = null;

						// Check if Dbus is available
						if ( isProcessActive("dbus-daemon") ) {
							providerName = "Dbus";
							provider = new Provider() {

								private static final String KEY = "/org/freedesktop/compiz/workarounds/allscreens/legacy_fullscreen";

								public boolean hasLegacyFullscreenSupport() throws LWJGLException {
									final List output = Compiz.run(
										"dbus-send", "--print-reply", "--type=method_call", "--dest=org.freedesktop.compiz", KEY, "org.freedesktop.compiz.get"
									);

									if ( output == null || output.size() < 2 )
										throw new LWJGLException("Invalid Dbus reply.");

									String line = (String)output.get(0);

									if ( !line.startsWith("method return") )
										throw new LWJGLException("Invalid Dbus reply.");

									line = ((String)output.get(1)).trim(); // value
									if ( !line.startsWith("boolean") || line.length() < 12)
										throw new LWJGLException("Invalid Dbus reply.");

									return "true".equalsIgnoreCase(line.substring("boolean".length() + 1));
								}

								public void setLegacyFullscreenSupport(final boolean state) throws LWJGLException {
									if ( Compiz.run(
										"dbus-send", "--type=method_call", "--dest=org.freedesktop.compiz", KEY, "org.freedesktop.compiz.set", "boolean:" + Boolean.toString(state)
									) == null )
										throw new LWJGLException("Failed to apply Compiz LFS workaround.");
								}
							};
						} else {
							try {
								// Check if Gconf is available
								Runtime.getRuntime().exec("gconftool");

								providerName = "gconftool";
								provider = new Provider() {

									private static final String KEY = "/apps/compiz/plugins/workarounds/allscreens/options/legacy_fullscreen";

									public boolean hasLegacyFullscreenSupport() throws LWJGLException {
										final List output = Compiz.run(new String[] {
											"gconftool", "-g", KEY
										});

										if ( output == null || output.size() == 0 )
											throw new LWJGLException("Invalid gconftool reply.");

										return Boolean.parseBoolean(((String)output.get(0)).trim());
									}

									public void setLegacyFullscreenSupport(final boolean state) throws LWJGLException {
										if ( Compiz.run(new String[] {
											"gconftool", "-s", KEY, "-s", Boolean.toString(state), "-t", "bool"
										}) == null )
											throw new LWJGLException("Failed to apply Compiz LFS workaround.");

										if ( state ) {
											try {
												// gconftool will not apply the workaround immediately, sleep a bit
												// to make sure it will be ok when we create the window.
												Thread.sleep(200); // 100 is too low, 150 works, set to 200 to be safe.
											} catch (InterruptedException e) {
												e.printStackTrace();
											}
										}
									}
								};
							} catch (IOException e) {
								// Ignore
							}
						}

						if ( provider != null && !provider.hasLegacyFullscreenSupport() ) { // No need to do anything if LFS is already enabled.
							applyFix = true;
							LWJGLUtil.log("Using " + providerName + " to apply Compiz LFS workaround.");
						}
					} catch (LWJGLException e) {
						// Ignore
					} finally {
						return null;
					}
				}
			});
		}

		static void setLegacyFullscreenSupport(final boolean enabled) {
			if ( !applyFix )
				return;

			AccessController.doPrivileged(new PrivilegedAction<Object>() {
				public Object run() {
					try {
						provider.setLegacyFullscreenSupport(enabled);
					} catch (LWJGLException e) {
						LWJGLUtil.log("Failed to change Compiz Legacy Fullscreen Support. Reason: " + e.getMessage());
					}
					return null;
				}
			});
		}

		private static List<String> run(final String... command) throws LWJGLException {
			final List<String> output = new ArrayList<String>();

			try {
				final Process p = Runtime.getRuntime().exec(command);
				try {
					final int exitValue = p.waitFor();
					if ( exitValue != 0 )
						return null;
				} catch (InterruptedException e) {
					throw new LWJGLException("Process interrupted.", e);
				}

				final BufferedReader br = new BufferedReader(new InputStreamReader(p.getInputStream()));

				String line;
				while ( (line = br.readLine()) != null )
					output.add(line);

				br.close();
			} catch (final IOException e) {
				throw new LWJGLException("Process failed.", e);
			}

			return output;
		}

		private static boolean isProcessActive(final String processName) throws LWJGLException {
			final List<String> output = run(new String[] { "ps", "-C", processName });
			if ( output == null )
				return false;

			for ( final String line : output ) {
				if ( line.contains(processName) )
					return true;
			}

			return false;
		}

		private interface Provider {

			boolean hasLegacyFullscreenSupport() throws LWJGLException;

			void setLegacyFullscreenSupport(boolean state) throws LWJGLException;

		}
	}

}
