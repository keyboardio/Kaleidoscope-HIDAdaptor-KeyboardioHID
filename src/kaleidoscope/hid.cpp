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
#include "BootKeyboard/BootKeyboard.h"

namespace kaleidoscope {
namespace hid {

__attribute__((weak))
void initializeKeyboard() {
  Keyboard.begin();
  BootKeyboard.begin();
}

__attribute__((weak))
void pressRawKey(Key mappedKey) {
  if (BootKeyboard.getProtocol() == HID_BOOT_PROTOCOL)
    BootKeyboard.press(mappedKey.keyCode);

  Keyboard.press(mappedKey.keyCode);
}

__attribute__((weak))
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

__attribute__((weak))
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

__attribute__((weak))
void releaseRawKey(Key mappedKey) {
  if (BootKeyboard.getProtocol() == HID_BOOT_PROTOCOL)
    BootKeyboard.release(mappedKey.keyCode);

  Keyboard.release(mappedKey.keyCode);
}

__attribute__((weak))
void releaseAllKeys() {
  BootKeyboard.releaseAll();
  Keyboard.releaseAll();
  ConsumerControl.releaseAll();
}

__attribute__((weak))
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

__attribute__((weak))
boolean isModifierKeyActive(Key mappedKey) {
  if (BootKeyboard.getProtocol() == HID_BOOT_PROTOCOL)
    return BootKeyboard.isModifierActive(mappedKey.keyCode);

  return Keyboard.isModifierActive(mappedKey.keyCode);
}

__attribute__((weak))
boolean wasModifierKeyActive(Key mappedKey) {
  if (BootKeyboard.getProtocol() == HID_BOOT_PROTOCOL)
    return BootKeyboard.wasModifierActive(mappedKey.keyCode);

  return Keyboard.wasModifierActive(mappedKey.keyCode);
}

__attribute__((weak))
uint8_t getKeyboardLEDs() {
  if (BootKeyboard.getProtocol() == HID_BOOT_PROTOCOL)
    return BootKeyboard.getLeds();

  return Keyboard.getLEDs();
}


__attribute__((weak))
void sendKeyboardReport() {
  if (BootKeyboard.getProtocol() == HID_BOOT_PROTOCOL) {
    BootKeyboard.sendReport();
    return;
  }

  Keyboard.sendReport();
  ConsumerControl.sendReport();
}

__attribute__((weak))
void initializeConsumerControl() {
  ConsumerControl.begin();
}

__attribute__((weak))
void pressConsumerControl(Key mappedKey) {
  ConsumerControl.press(CONSUMER(mappedKey));
}

__attribute__((weak))
void releaseConsumerControl(Key mappedKey) {
  ConsumerControl.release(CONSUMER(mappedKey));
}


__attribute__((weak))
void initializeSystemControl() {
  SystemControl.begin();
}

__attribute__((weak))
void pressSystemControl(Key mappedKey) {
  SystemControl.press(mappedKey.keyCode);
}

__attribute__((weak))
void releaseSystemControl(Key mappedKey) {
  SystemControl.release();
}


// Mouse events

__attribute__((weak))
void initializeMouse() {
  Mouse.begin();
}

__attribute__((weak))
void moveMouse(signed char x, signed char y, signed char vWheel, signed char hWheel) {
  Mouse.move(x, y, vWheel, hWheel);
}

__attribute__((weak))
void clickMouseButtons(uint8_t buttons) {
  Mouse.click(buttons);
}

__attribute__((weak))
void pressMouseButtons(uint8_t buttons) {
  Mouse.press(buttons);
}

__attribute__((weak))
void releaseMouseButtons(uint8_t buttons) {
  Mouse.release(buttons);
}

__attribute__((weak))
void releaseAllMouseButtons(void) {
  Mouse.releaseAll();
}

__attribute__((weak))
void sendMouseReport(void) {
  Mouse.sendReport();
}

/** SingleAbsolute mouse (grapahics tablet) events */

__attribute__((weak))
void initializeAbsoluteMouse() {
  SingleAbsoluteMouse.begin();
}

__attribute__((weak))
void moveAbsoluteMouse(signed char x, signed char y, signed char wheel) {
  SingleAbsoluteMouse.move(x, y, wheel);
}
__attribute__((weak))
void moveAbsoluteMouseTo(uint16_t x, uint16_t y, signed char wheel) {
  SingleAbsoluteMouse.moveTo(x, y, wheel);
}

__attribute__((weak))
void clickAbsoluteMouseButtons(uint8_t buttons) {
  SingleAbsoluteMouse.click(buttons);
}

__attribute__((weak))
void pressAbsoluteMouseButtons(uint8_t buttons) {
  SingleAbsoluteMouse.press(buttons);
}

__attribute__((weak))
void releaseAbsoluteMouseButtons(uint8_t buttons) {
  SingleAbsoluteMouse.release(buttons);
}

}
};
