#pragma once
#include <BleKeyboard.h>
#include "menu.h"

static BleKeyboard _ble("ESP32 MacroBar", "DIY", 100);

inline void bleInit() {
  _ble.begin();
}

inline bool bleReady() {
  return _ble.isConnected();
}

// Open a URL via Spotlight (Cmd+Space → type URL → Enter)
static void openURL(const char* url) {
  _ble.press(KEY_LEFT_GUI);
  _ble.press(' ');
  delay(50);
  _ble.releaseAll();
  delay(400);
  _ble.print(url);
  delay(100);
  _ble.write(KEY_RETURN);
}

inline void executeAction(uint8_t page, uint8_t item) {
  if (!bleReady()) return;

  // Page 0: Media
  if (page == 0) {
    if (item == 0) _ble.write(KEY_MEDIA_VOLUME_UP);
    if (item == 1) _ble.write(KEY_MEDIA_VOLUME_DOWN);
    if (item == 2) _ble.write(KEY_MEDIA_MUTE);
  }

  // Page 1: Display
  if (page == 1) {
    if (item == 0) _ble.write(KEY_MEDIA_BRIGHTNESS_UP);
    if (item == 1) _ble.write(KEY_MEDIA_BRIGHTNESS_DOWN);
  }

  // Page 2: Sites
  if (page == 2) {
    if (item == 0) openURL("youtube.com\n");
    if (item == 1) openURL("github.com\n");
    if (item == 2) openURL("chatgpt.com\n");
  }

  // Page 3: System
  if (page == 3) {
    if (item == 0) {
      // Lock screen: Cmd+Ctrl+Q
      _ble.press(KEY_LEFT_GUI);
      _ble.press(KEY_LEFT_CTRL);
      _ble.press('q');
      delay(50);
      _ble.releaseAll();
    }
    if (item == 1) {
      // Screenshot: Cmd+Shift+4
      _ble.press(KEY_LEFT_GUI);
      _ble.press(KEY_LEFT_SHIFT);
      _ble.press('4');
      delay(50);
      _ble.releaseAll();
    }
  }
}
