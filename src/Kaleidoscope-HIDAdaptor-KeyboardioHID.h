/* -*- mode: c++ -*-
 * Kaleidoscope-HIDAdaptor-KeyboardioHID -- KeyboardioHID HID Adaptor
 * Copyright (C) Jesse Vincent <jesse@keyboard.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#if KALEIDOSCOPE_HIDADAPTOR_DISABLE_KEYBOARD_BOOT_PROTOCOL
#define KALEIDOSCOPE_HIDADAPTOR_ENABLE_KEYBOARD_BOOT_PROTOCOL 0
#else
#define KALEIDOSCOPE_HIDADAPTOR_ENABLE_KEYBOARD_BOOT_PROTOCOL 1
#endif

#if KALEIDOSCOPE_HIDADAPTOR_DISABLE_ABSOLUTE_MOUSE
#define KALEIDOSCOPE_HIDADAPTOR_ENABLE_ABSOLUTE_MOUSE 0
#else
#define KALEIDOSCOPE_HIDADAPTOR_ENABLE_ABSOLUTE_MOUSE 1
#endif

#if KALEIDOSCOPE_HIDADAPTOR_DISABLE_MOUSE
#define KALEIDOSCOPE_HIDADAPTOR_ENABLE_MOUSE 0
#else
#define KALEIDOSCOPE_HIDADAPTOR_ENABLE_MOUSE 1
#endif
