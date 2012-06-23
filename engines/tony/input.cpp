/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 * This code is based on original Tony Tough source code
 *
 * Copyright (c) 1997-2003 Nayma Software
 */

#include "tony/gfxengine.h"
#include "tony/tony.h"

namespace Tony {

RMInput::RMInput() {
	// Setup mouse fields
	_clampMouse = false;
	_mousePos.set(0, 0);
	_leftButton = _rightButton = false;
	_leftClickMouse = _leftReleaseMouse = false;
	_rightClickMouse = _rightReleaseMouse = false;

	Common::fill((byte *)&_event, (byte *)&_event + sizeof(Common::Event), 0);

	// Setup keyboard fields
	Common::fill(&_keyDown[0], &_keyDown[350], 0);
}

RMInput::~RMInput() {
}

void RMInput::poll() {
	_leftClickMouse = _leftReleaseMouse = _rightClickMouse = _rightReleaseMouse = false;

	// Get pending events
	while (g_system->getEventManager()->pollEvent(_event) && !_vm->shouldQuit()) {
		switch (_event.type) {
		case Common::EVENT_MOUSEMOVE:
		case Common::EVENT_LBUTTONDOWN:
		case Common::EVENT_LBUTTONUP:
		case Common::EVENT_RBUTTONDOWN:
		case Common::EVENT_RBUTTONUP:
			_mousePos.set(_event.mouse.x, _event.mouse.y);

			if (_event.type == Common::EVENT_LBUTTONDOWN) {
				_leftButton = true;
				_leftClickMouse = true;
			} else if (_event.type == Common::EVENT_LBUTTONUP) {
				_leftButton = false;
				_leftReleaseMouse = true;
			} else if (_event.type == Common::EVENT_RBUTTONDOWN) {
				_rightButton = true;
				_rightClickMouse = true;
			} else if (_event.type == Common::EVENT_RBUTTONUP) {
				_rightButton = false;
				_rightReleaseMouse = true;
			} else
				continue;

			// Since a mouse button has changed, don't do any further event processing this frame
			return;

		case Common::EVENT_KEYDOWN:
			// Check for debugger
			if ((_event.kbd.keycode == Common::KEYCODE_d) && (_event.kbd.flags & Common::KBD_CTRL)) {
				// Attach to the debugger
				_vm->_debugger->attach();
				_vm->_debugger->onFrame();
			} else {
				// Flag the given key as being down
				_keyDown[(int)_event.kbd.keycode] = true;
			}
			return;

		case Common::EVENT_KEYUP:
			_keyDown[(int)_event.kbd.keycode] = false;
			return;

		default:
			break;
		}
	}
}

void RMInput::init() {
}

void RMInput::close() {
}

bool RMInput::mouseLeft() {
	return _leftButton;
}

bool RMInput::mouseRight() {
	return _rightButton;
}

/**
 * Return true if a key has been pressed
 */
bool RMInput::getAsyncKeyState(Common::KeyCode kc) {
	// The act of testing for a particular key automatically clears the state, to prevent
	// the same key being registered in multiple different frames
	bool result = _keyDown[(int)kc];
	_keyDown[(int)kc] = false;
	return result;
}

} // End of namespace Tony