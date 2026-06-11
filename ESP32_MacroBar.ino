// ESP32 MacroBar — Main Entry Point
#include "menu.h"
#include "joystick.h"
#include "lcd_display.h"
#include "ble_hid.h"

void setup() {
  Serial.begin(115200);
  joystickInit();
  lcdInit();
  bleInit();
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
  }

  if (buttonLongPressed()) {
    resetToFirstPage();
  }

  lcdRender(currentPage(), currentItem());
  delay(100);
}
