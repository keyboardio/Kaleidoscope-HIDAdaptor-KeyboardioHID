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

// The functions in this namespace are primarily to solve the problem of rollover from a
// key with a modifier flag (e.g. `LSHIFT(Key_T)`) to one without (e.g. `Key_H`), which
// used to result in the mod flag being applied to keys other than the one with the
// flag. By using `mod_flags_allowed`, we can prohibit any modifier flags that aren't in
// the key most recently pressed, fixing the rollover problem, and getting the intended
// `The` instead of `THe`. This was particularly noticeable to people who were using a
// separate layer for `shift`, and a OneShot layer key to activate it, causing what looked
// like a OneShot bug.

// A bitfield of the current modifier flags from keys processed so far in the current
// keyswitch scan:
static byte requested_mod_flags{0};

// The current set of modifier flags that will be allowed to be added to the upcoming
// keyboard HID report. This should be set by the latest key pressed:
static byte mod_flags_allowed{0};

// The keycode of the last key to toggle on. This get
static byte toggled_on_keycode{0};

// This returns true if the key is a keyboard key, and its keycode is a modifier.
inline
bool isPureModifier(Key mappedKey) {
  // If it's not a keyboard key, return false
  if (mappedKey.flags & (SYNTHETIC | RESERVED))
    return false;
  return (mappedKey.keyCode >= HID_KEYBOARD_FIRST_MODIFIER &&
          mappedKey.keyCode <= HID_KEYBOARD_LAST_MODIFIER);
}

// This function adds modifier flags to the bitfield for current modifier flags that will
// be added to the report after all keys are scanned.
inline
void requestModFlags(byte flags) {
  requested_mod_flags |= flags;
}

// This function actually adds the modifier flags to the upcoming report.
inline
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

// Called from handleKeyswitchEventDefault when a key toggles on. This sets the mask of
// modifier flags that are allowed to be added to the upcoming report to the ones in the
// key that just toggled on, so when we roll over from a key with a modifier flag to one
// without it, that modifier flag won't affect the new keypress.
void pressToggledOnKey(Key mappedKey) {
  mod_flags_allowed = mappedKey.flags;
  toggled_on_keycode = mappedKey.keyCode;
  pressKey(mappedKey);
}

void pressRawKey(Key mappedKey) {
  WITH_BOOTKEYBOARD_PROTOCOL {
    BootKeyboard.press(mappedKey.keyCode);
    return;
  }

  Keyboard.press(mappedKey.keyCode);
}

void pressKey(Key mappedKey) {
  // If the key we're processing is a "pure" modifier key, its modifier flags are added
  // directly to the report along with the modifier from its keycode byte. We assume that
  // the flagged modifiers are intended to also modify other keys pressed while this key
  // is held, so they are allowed unconditionally. If it's not a "pure" modifier key, we
  // add them to requested_mod_flags, and only allow them to affect the report if the latest
  // keypress includes those modifiers.
  if (isPureModifier(mappedKey)) {
    pressModFlags(mappedKey.flags);
  } else {
    requestModFlags(mappedKey.flags);
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

  requested_mod_flags = 0;
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

  // Before sending the report, we add any modifier flags that are currently allowed,
  // based on the latest keypress:
  pressModFlags(requested_mod_flags & mod_flags_allowed);


  // If a key has just toggled on in this cycle, we might need to send an extra HID report
  // to the host, because that key might have the same keycode as another key that was
  // already in the report on the previous cycle. For example, a user could have two
  // `Key_E` keys in their keymap, in order to avoid repeating the same key with one
  // finger. Or one might have a `LCTRL(Key_S)` and a plain `Key_S`, and have a reason to
  // press them in rapid succession. In order to make this work, we need to call
  // `release()` & `sendReport()` to send a release event to the host so that its normal
  // repeat-rate-limiting behaviour won't effectively mask the second keypress. Then we
  // call `press()` to add the keycode back in before sending the normal report.
  //
  // In most cases, this won't result in any difference from the previous report (because
  // the newly-toggled-on keycode won't be in the previous report), so no extra report
  // will be sent (because we suppress duplicate reports in KeyboardioHID). If there is a
  // difference in the modifiers byte, an extra report would be sent later, regardless
  // (also in KeyboardioHID).
  WITH_BOOTKEYBOARD_PROTOCOL {
    if (toggled_on_keycode) {
      BootKeyboard.release(toggled_on_keycode);
      BootKeyboard.sendReport();
      BootKeyboard.press(toggled_on_keycode);
      toggled_on_keycode = 0;
    }

    BootKeyboard.sendReport();
    return;
  }

  if (toggled_on_keycode) {
    Keyboard.release(toggled_on_keycode);
    Keyboard.sendReport();
    Keyboard.press(toggled_on_keycode);
    toggled_on_keycode = 0;
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
