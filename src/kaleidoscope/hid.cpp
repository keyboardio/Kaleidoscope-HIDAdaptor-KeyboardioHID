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

#include "Kaleidoscope.h"
#include "KeyboardioHID.h"

namespace kaleidoscope {
namespace hid {

void initializeKeyboard() {
  Kaleidoscope.hid().keyboard().setup();
}

void pressKey(Key pressed_key, boolean toggled_on) {
  Kaleidoscope.hid().keyboard().pressKey(pressed_key, toggled_on);
}

void pressRawKey(Key pressed_key) {
  Kaleidoscope.hid().keyboard().pressRawKey(pressed_key);
}

void releaseRawKey(Key released_key) {
  Kaleidoscope.hid().keyboard().releaseRawKey(released_key);
}

void releaseAllKeys() {
  Kaleidoscope.hid().keyboard().releaseAllKeys();
}

void releaseKey(Key released_key) {
  Kaleidoscope.hid().keyboard().releaseKey(released_key);
}

boolean isModifierKeyActive(Key modifier_key) {
  return Kaleidoscope.hid().keyboard().isModifierKeyActive(modifier_key);
}

boolean wasModifierKeyActive(Key modifier_key) {
  return Kaleidoscope.hid().keyboard().wasModifierKeyActive(modifier_key);
}

boolean isAnyModifierKeyActive() {
  return Kaleidoscope.hid().keyboard().isAnyModifierKeyActive();
}

boolean wasAnyModifierKeyActive() {
  return Kaleidoscope.hid().keyboard().wasAnyModifierKeyActive();
}

uint8_t getKeyboardLEDs() {
  return Kaleidoscope.hid().keyboard().getKeyboardLEDs();
}

void sendKeyboardReport() {
  Kaleidoscope.hid().keyboard().sendReport();
}

void initializeConsumerControl() {
  // Done by .keyboard().setup()
}

void pressConsumerControl(Key mappedKey) {
  Kaleidoscope.hid().keyboard().pressConsumerControl(mappedKey);
}

void releaseConsumerControl(Key mappedKey) {
  Kaleidoscope.hid().keyboard().releaseConsumerControl(mappedKey);
}


void initializeSystemControl() {
  // Done by .keyboard().setup()
}

void pressSystemControl(Key mappedKey) {
  Kaleidoscope.hid().keyboard().pressSystemControl(mappedKey);
}

void releaseSystemControl(Key mappedKey) {
  Kaleidoscope.hid().keyboard().releaseSystemControl(mappedKey);
}


// Mouse events

void initializeMouse() {
  Kaleidoscope.hid().mouse().setup();
}

void moveMouse(signed char x, signed char y, signed char vWheel, signed char hWheel) {
  Kaleidoscope.hid().mouse().move(x, y, vWheel, hWheel);
}

void stopMouse(bool x, bool y, bool vWheel, bool hWheel) {
  Kaleidoscope.hid().mouse().stop(x, y, vWheel, hWheel);
}

void clickMouseButtons(uint8_t buttons) {
  Kaleidoscope.hid().mouse().clickButtons(buttons);
}

void pressMouseButtons(uint8_t buttons) {
  Kaleidoscope.hid().mouse().pressButtons(buttons);
}

void releaseMouseButtons(uint8_t buttons) {
  Kaleidoscope.hid().mouse().releaseButtons(buttons);
}

void releaseAllMouseButtons(void) {
  Kaleidoscope.hid().mouse().releaseAllButtons();
}

void sendMouseReport(void) {
  Kaleidoscope.hid().mouse().sendReport();
}

/** SingleAbsolute mouse (grapahics tablet) events */

void initializeAbsoluteMouse() {
  Kaleidoscope.hid().absoluteMouse().setup();
}

void moveAbsoluteMouse(signed char x, signed char y, signed char wheel) {
  Kaleidoscope.hid().absoluteMouse().move(x, y, wheel);
}
void moveAbsoluteMouseTo(uint16_t x, uint16_t y, signed char wheel) {
  Kaleidoscope.hid().absoluteMouse().moveTo(x, y, wheel);
}

void clickAbsoluteMouseButtons(uint8_t buttons) {
  Kaleidoscope.hid().absoluteMouse().clickButtons(buttons);
}

void pressAbsoluteMouseButtons(uint8_t buttons) {
  Kaleidoscope.hid().absoluteMouse().pressButtons(buttons);
}

void releaseAbsoluteMouseButtons(uint8_t buttons) {
  Kaleidoscope.hid().absoluteMouse().releaseButtons(buttons);
}

}
}
