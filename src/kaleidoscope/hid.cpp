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

#include "Kaleidoscope.h"
#include "KeyboardioHID.h"

#if KALEIDOSCOPE_HIDADAPTOR_ENABLE_KEYBOARD_BOOT_PROTOCOL
#include "BootKeyboard/BootKeyboard.h"

/* These two macros are here to make code look nicer. The goal is that unless
 * boot protocol support is disabled (it is enabled by default), we'll include
 * the BootKeyboard fallback mechanism. Otherwise, we wrap calls in "if (0)"
 * blocks, and let the compiler remove them.
 */

/** Fall back to BootKeyboard, if the HID protocol is Boot.
 * Requires a block of code to follow.
 */
#define WITH_BOOTKEYBOARD_PROTOCOL if (BootKeyboard.getProtocol() == HID_BOOT_PROTOCOL)
/** Do something with BootKeyboard, if it is enabled.
 * Requires a block of code to follow.
 */
#define WITH_BOOTKEYBOARD

#else /* KALEIDOSCOPE_HIDADAPTOR_ENABLE_KEYBOARD_BOOT_PROTOCOL unset */

/* Wrap both macros in "if (0)", so that the compiler removes any code that
 * follows.
 */
#define WITH_BOOTKEYBOARD_PROTOCOL if (0)
#define WITH_BOOTKEYBOARD if (0)

#endif

namespace kaleidoscope {
namespace hid {

// Anonymous namespace for internal variables and helper functions
namespace {

static byte mod_flags{0};
static byte mod_flags_allowed{0};
static byte toggled_on_keycode{0};

bool isPureModifier(Key mappedKey) {
  if (mappedKey.flags & (SYNTHETIC | RESERVED))
    return false;
  byte offset_keycode = mappedKey.keyCode - Key_LeftControl.keyCode;
  return (offset_keycode < 8);
}

void addModFlags(byte flags) {
  mod_flags |= flags;
}

void pressModFlags(byte flags) {
  if (flags & SHIFT_HELD) {
    pressRawKey(Key_LeftShift);
  }
  if (flags & CTRL_HELD) {
    pressRawKey(Key_LeftControl);
  }
  if (flags & LALT_HELD) {
    pressRawKey(Key_LeftAlt);
  }
  if (flags & RALT_HELD) {
    pressRawKey(Key_RightAlt);
  }
  if (flags & GUI_HELD) {
    pressRawKey(Key_LeftGui);
  }
}

} // anonymous namespace

void initializeKeyboard() {
  Keyboard.begin();
  WITH_BOOTKEYBOARD {
    BootKeyboard.begin();
  }
}

void setLastKeyPress(Key mappedKey) {
  mod_flags_allowed = mappedKey.flags;
  mod_flags = mappedKey.flags;
  toggled_on_keycode = mappedKey.keyCode;
}

void pressRawKey(Key mappedKey) {
  WITH_BOOTKEYBOARD_PROTOCOL {
    BootKeyboard.press(mappedKey.keyCode);
    return;
  }

  Keyboard.press(mappedKey.keyCode);
}

void pressKey(Key mappedKey) {
  if (isPureModifier(mappedKey)) {
    pressModFlags(mappedKey.flags);
  } else {
    addModFlags(mappedKey.flags);
  }

  pressRawKey(mappedKey);
}

void releaseRawKey(Key mappedKey) {
  WITH_BOOTKEYBOARD_PROTOCOL {
    BootKeyboard.release(mappedKey.keyCode);
    return;
  }

  Keyboard.release(mappedKey.keyCode);
}

void releaseAllKeys() {
  WITH_BOOTKEYBOARD {
    BootKeyboard.releaseAll();
  }

  mod_flags = 0;
  toggled_on_keycode = 0;
  Keyboard.releaseAll();
  ConsumerControl.releaseAll();
}

void releaseKey(Key mappedKey) {
  if (mappedKey.flags & SHIFT_HELD) {
    releaseRawKey(Key_LeftShift);
  }
  if (mappedKey.flags & CTRL_HELD) {
    releaseRawKey(Key_LeftControl);
  }
  if (mappedKey.flags & LALT_HELD) {
    releaseRawKey(Key_LeftAlt);
  }
  if (mappedKey.flags & RALT_HELD) {
    releaseRawKey(Key_RightAlt);
  }
  if (mappedKey.flags & GUI_HELD) {
    releaseRawKey(Key_LeftGui);
  }
  releaseRawKey(mappedKey);
}

boolean isModifierKeyActive(Key mappedKey) {
  WITH_BOOTKEYBOARD_PROTOCOL {
    return BootKeyboard.isModifierActive(mappedKey.keyCode);
  }

  return Keyboard.isModifierActive(mappedKey.keyCode);
}

boolean wasModifierKeyActive(Key mappedKey) {
  WITH_BOOTKEYBOARD_PROTOCOL {
    return BootKeyboard.wasModifierActive(mappedKey.keyCode);
  }

  return Keyboard.wasModifierActive(mappedKey.keyCode);
}

uint8_t getKeyboardLEDs() {
  WITH_BOOTKEYBOARD_PROTOCOL {
    return BootKeyboard.getLeds();
  }

  return Keyboard.getLEDs();
}


void sendKeyboardReport() {

  pressModFlags(mod_flags & mod_flags_allowed);

  WITH_BOOTKEYBOARD_PROTOCOL {
    if (toggled_on_keycode) {
      BootKeyboard.release(toggled_on_keycode);
      BootKeyboard.sendReport();
      BootKeyboard.press(toggled_on_keycode);
    }
    toggled_on_keycode = 0;

    BootKeyboard.sendReport();
    return;
  }

  if (toggled_on_keycode) {
    Keyboard.release(toggled_on_keycode);
    Keyboard.sendReport();
    Keyboard.press(toggled_on_keycode);
  }
  toggled_on_keycode = 0;

  Keyboard.sendReport();
  ConsumerControl.sendReport();
}

void initializeConsumerControl() {
  ConsumerControl.begin();
}

void pressConsumerControl(Key mappedKey) {
  ConsumerControl.press(CONSUMER(mappedKey));
}

void releaseConsumerControl(Key mappedKey) {
  ConsumerControl.release(CONSUMER(mappedKey));
}


void initializeSystemControl() {
  SystemControl.begin();
}

void pressSystemControl(Key mappedKey) {
  SystemControl.press(mappedKey.keyCode);
}

void releaseSystemControl(Key mappedKey) {
  SystemControl.release();
}


// Mouse events

#if KALEIDOSCOPE_HIDADAPTOR_ENABLE_MOUSE
void initializeMouse() {
  Mouse.begin();
}

void moveMouse(signed char x, signed char y, signed char vWheel, signed char hWheel) {
  Mouse.move(x, y, vWheel, hWheel);
}

void clickMouseButtons(uint8_t buttons) {
  Mouse.click(buttons);
}

void pressMouseButtons(uint8_t buttons) {
  Mouse.press(buttons);
}

void releaseMouseButtons(uint8_t buttons) {
  Mouse.release(buttons);
}

void releaseAllMouseButtons(void) {
  Mouse.releaseAll();
}

void sendMouseReport(void) {
  Mouse.sendReport();
}
#else
void initializeMouse() {
}

void moveMouse(signed char x, signed char y, signed char vWheel, signed char hWheel) {
}

void clickMouseButtons(uint8_t buttons) {
}

void pressMouseButtons(uint8_t buttons) {
}

void releaseMouseButtons(uint8_t buttons) {
}

void releaseAllMouseButtons(void) {
}

void sendMouseReport(void) {
}
#endif

/** SingleAbsolute mouse (grapahics tablet) events */

#if KALEIDOSCOPE_HIDADAPTOR_ENABLE_ABSOLUTE_MOUSE && KALEIDOSCOPE_HIDADAPTOR_ENABLE_MOUSE

void initializeAbsoluteMouse() {
  SingleAbsoluteMouse.begin();
}

void moveAbsoluteMouse(signed char x, signed char y, signed char wheel) {
  SingleAbsoluteMouse.move(x, y, wheel);
}
void moveAbsoluteMouseTo(uint16_t x, uint16_t y, signed char wheel) {
  SingleAbsoluteMouse.moveTo(x, y, wheel);
}

void clickAbsoluteMouseButtons(uint8_t buttons) {
  SingleAbsoluteMouse.click(buttons);
}

void pressAbsoluteMouseButtons(uint8_t buttons) {
  SingleAbsoluteMouse.press(buttons);
}

void releaseAbsoluteMouseButtons(uint8_t buttons) {
  SingleAbsoluteMouse.release(buttons);
}

#else

void initializeAbsoluteMouse() {
}

void moveAbsoluteMouse(signed char x, signed char y, signed char wheel) {
}
void moveAbsoluteMouseTo(uint16_t x, uint16_t y, signed char wheel) {
}

void clickAbsoluteMouseButtons(uint8_t buttons) {
}

void pressAbsoluteMouseButtons(uint8_t buttons) {
}

void releaseAbsoluteMouseButtons(uint8_t buttons) {
}

#endif

}
};
