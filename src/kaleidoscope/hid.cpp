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

#if !KALEIDOSCOPE_HIDADAPTOR_DISABLE_KEYBOARD_BOOT_PROTOCOL
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

#else /* KALEIDOSCOPE_HIDADAPTOR_DISABLE_KEYBOARD_BOOT_PROTOCOL set */

/* Wrap both macros in "if (0)", so that the compiler removes any code that
 * follows.
 */
#define WITH_BOOTKEYBOARD_PROTOCOL if (0)
#define WITH_BOOTKEYBOARD if (0)

#endif

namespace kaleidoscope {
namespace hid {

void initializeKeyboard() {
  Keyboard.begin();
  WITH_BOOTKEYBOARD {
    BootKeyboard.begin();
  }
}

void pressRawKey(Key mappedKey) {
  WITH_BOOTKEYBOARD_PROTOCOL {
    BootKeyboard.press(mappedKey.keyCode);
  }

  Keyboard.press(mappedKey.keyCode);
}

void _pressModifierKey(Key mappedKey) {
  pressRawKey(mappedKey);

  /* On at least ChromeOS 51-60, first sending a "press shift" key event in the same report
   * as the key report for some shifted keys causes the shift event to be missed.
   *
   * Specifically, Shift + [ in the same key report do not genereate a {
   *
   * This workaround causes an extra key report to be sent to toggle on the modifier key
   * before sending the key event we're trying to modify as part of a key report
   *
   * I (Jesse) don't believe we need a similar workaround for toggling the modifer _off_
   */

  if (!wasModifierKeyActive(mappedKey)) {
    sendKeyboardReport();
  }
}

void pressKey(Key mappedKey) {
  if (mappedKey.flags & SHIFT_HELD) {
    _pressModifierKey(Key_LeftShift);
  }
  if (mappedKey.flags & CTRL_HELD) {
    _pressModifierKey(Key_LeftControl);
  }
  if (mappedKey.flags & LALT_HELD) {
    _pressModifierKey(Key_LeftAlt);
  }
  if (mappedKey.flags & RALT_HELD) {
    _pressModifierKey(Key_RightAlt);
  }
  if (mappedKey.flags & GUI_HELD) {
    _pressModifierKey(Key_LeftGui);
  }

  pressRawKey(mappedKey);
}

void releaseRawKey(Key mappedKey) {
  WITH_BOOTKEYBOARD_PROTOCOL {
    BootKeyboard.release(mappedKey.keyCode);
  }

  Keyboard.release(mappedKey.keyCode);
}

void releaseAllKeys() {
  WITH_BOOTKEYBOARD {
    BootKeyboard.releaseAll();
  }

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
  WITH_BOOTKEYBOARD_PROTOCOL {
    BootKeyboard.sendReport();
    return;
  }

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

/** SingleAbsolute mouse (grapahics tablet) events */

#if !KALEIDOSCOPE_HIDADAPTOR_DISABLE_ABSOLUTE_MOUSE

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
