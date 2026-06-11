#pragma once
#include <LiquidCrystal.h>
#include "menu.h"

// RS, E, D4, D5, D6, D7
static LiquidCrystal _lcd(19, 23, 18, 17, 16, 15);

static uint8_t _lastPage = 255;
static uint8_t _lastItem = 255;

inline void lcdInit() {
  _lcd.begin(16, 2);
  _lcd.clear();
}

inline void lcdRender(uint8_t page, uint8_t item) {
  if (page == _lastPage && item == _lastItem) return;
  _lastPage = page;
  _lastItem = item;

  const MenuPage& p = PAGES[page];

  _lcd.setCursor(0, 0);
  _lcd.print(p.title);
  // Pad to clear leftover chars
  for (int i = strlen(p.title); i < 16; i++) _lcd.print(' ');

  _lcd.setCursor(0, 1);
  char buf[17];
  snprintf(buf, sizeof(buf), ">%-15s", p.items[item].label);
  _lcd.print(buf);
}
