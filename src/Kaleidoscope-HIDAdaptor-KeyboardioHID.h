/* -*- mode: c++ -*-
 * Kaleidoscope-HIDAdaptor-KeyboardioHID -- KeyboardioHID HID Adaptor
 * Copyright (C) Jesse Vincent <jesse@keyboard.io>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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
