// ESP32 MacroBar — Main Entry Point
#include "menu.h"
#include "joystick.h"
#include "lcd_display.h"
#include "ble_hid.h"
#include "dht_sensor.h"

#define SENSOR_PAGE 5

void setup() {
  Serial.begin(115200);
  joystickInit();
  lcdInit();
  bleInit();
  dhtInit();
  lcdRender(currentPage(), currentItem());
}

void loop() {
  Direction dir = joystickRead();

  switch (dir) {
    case DIR_LEFT:  prevPage(); break;
    case DIR_RIGHT: nextPage(); break;
    case DIR_UP:    prevItem(); break;
    case DIR_DOWN:  nextItem(); break;
    case DIR_NONE:  break;
  }

  if (buttonPressed()) {
    executeAction(currentPage(), currentItem());
    delay(500);  // cooldown after action to prevent joystick drift triggering next input
    return;
  }

  if (buttonLongPressed()) {
    resetToFirstPage();
    delay(500);
    return;
  }

  if (currentPage() == SENSOR_PAGE) {
    static unsigned long _lastDhtUpdate = 0;
    if (millis() - _lastDhtUpdate > 2000) {
      dhtRender();
      _lastDhtUpdate = millis();
    }
  } else {
    lcdRender(currentPage(), currentItem());
  }
  delay(100);
}
