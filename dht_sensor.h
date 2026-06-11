#pragma once
#include <DHT.h>
#include "lcd_display.h"

#define DHT_PIN  4
#define DHT_TYPE DHT11

static DHT _dht(DHT_PIN, DHT_TYPE);

inline void dhtInit() {
  _dht.begin();
}

inline void dhtRender() {
  float temp = _dht.readTemperature();
  float hum  = _dht.readHumidity();

  _lcd.setCursor(0, 0);
  if (isnan(temp) || isnan(hum)) {
    _lcd.print("Sensor Error    ");
    _lcd.setCursor(0, 1);
    _lcd.print("Check DHT11     ");
    return;
  }

  char line1[17];
  char line2[17];
  snprintf(line1, sizeof(line1), "Temp: %.1f C    ", temp);
  snprintf(line2, sizeof(line2), "Humi: %.1f %%   ", hum);
  _lcd.print(line1);
  _lcd.setCursor(0, 1);
  _lcd.print(line2);
}
