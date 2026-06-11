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

// Open a URL via Spotlight (Cmd+Space → type URL char by char → wait → Enter)
static void openURL(const char* url) {
  _ble.press(KEY_LEFT_GUI);
  _ble.press(' ');
  delay(50);
  _ble.releaseAll();
  delay(800);
  // Type each character slowly so BLE doesn't drop chars
  for (int i = 0; url[i] != '\0'; i++) {
    _ble.print(url[i]);
    delay(50);
  }
  delay(1500);
  _ble.write(KEY_RETURN);
}

inline void executeAction(uint8_t page, uint8_t item) {
  if (!bleReady()) return;

  // Page 0: Media
  if (page == 0) {
    if (item == 0) _ble.write(KEY_MEDIA_VOLUME_UP);
    if (item == 1) _ble.write(KEY_MEDIA_VOLUME_DOWN);
    if (item == 2) _ble.write(KEY_MEDIA_MUTE);
    if (item == 3) _ble.write(KEY_MEDIA_PLAY_PAUSE);
  }

  // Page 1: Media 2
  if (page == 1) {
    if (item == 0) _ble.write(KEY_MEDIA_PREVIOUS_TRACK);
    if (item == 1) _ble.write(KEY_MEDIA_NEXT_TRACK);
  }

  // Page 2: Sites
  if (page == 2) {
    if (item == 0) openURL("https://www.youtube.com");
    if (item == 1) openURL("https://www.github.com");
    if (item == 2) openURL("https://www.chatgpt.com");
    if (item == 3) openURL("https://www.claude.ai");
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

  // Page 4: Window
  if (page == 4) {
    if (item == 0) {
      // Mission Control: Ctrl+Up
      _ble.press(KEY_LEFT_CTRL);
      _ble.press(KEY_UP_ARROW);
      delay(50);
      _ble.releaseAll();
    }
    if (item == 1) {
      // Switch input method: Ctrl+Space
      _ble.press(KEY_LEFT_CTRL);
      _ble.press(' ');
      delay(50);
      _ble.releaseAll();
    }
    if (item == 2) {
      // Show Desktop: F11
      _ble.write(KEY_F11);
    }
  }
}
