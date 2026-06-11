# ESP32 MacroBar

A wireless Bluetooth macro controller built with ESP32, inspired by the Mac Touch Bar.  
Navigate pages and trigger actions using a joystick — no drivers required.

## Hardware

| Component | Details |
|-----------|---------|
| ESP32 Dev Board | Any common variant |
| LCD 1602A | 4-bit mode, direct pin connection |
| Keyes Joystick | X/Y analog axes + button |
| 10kΩ Potentiometer | LCD contrast adjustment |

## Wiring

### LCD 1602A → ESP32

| LCD Pin | Function | ESP32 |
|---------|----------|-------|
| VSS | GND | GND |
| VDD | 5V | VIN |
| V0 | Contrast | Potentiometer center |
| RS | Register Select | GPIO 19 |
| RW | Read/Write | GND |
| E | Enable | GPIO 23 |
| D4 | Data | GPIO 18 |
| D5 | Data | GPIO 17 |
| D6 | Data | GPIO 16 |
| D7 | Data | GPIO 15 |
| A | Backlight + | 3.3V |
| K | Backlight − | GND |

### Joystick → ESP32

| Joystick Pin | ESP32 |
|-------------|-------|
| VCC | 3.3V |
| GND | GND |
| VRX | GPIO 34 |
| VRY | GPIO 35 |
| SW | GPIO 32 |

## Menu Structure

```
Page 1: Media    →  Vol+ / Vol- / Mute
Page 2: Display  →  Brightness+ / Brightness-
Page 3: Sites    →  YouTube / GitHub / ChatGPT
Page 4: System   →  Lock Screen / Screenshot
```

## Controls

| Action | Result |
|--------|--------|
| Joystick Left / Right | Switch page |
| Joystick Up / Down | Switch item |
| Press button | Execute current action |
| Hold button (>1s) | Return to first page |

## Dependencies

- [ESP32 BLE Keyboard](https://github.com/T-vK/ESP32-BLE-Keyboard) by T-vK
- LiquidCrystal (Arduino built-in)
- Espressif ESP32 Board Package **2.x** (do not use 3.x)

## Project Structure

```
ESP32_MacroBar/
├── ESP32_MacroBar.ino   # Main entry point
├── menu.h               # Menu data and state machine
├── joystick.h           # Joystick input with debounce and long-press
├── lcd_display.h        # LCD rendering with dirty-flag updates
└── ble_hid.h            # BLE HID actions (media keys, shortcuts, URLs)
```

## How It Works

The device appears as a Bluetooth HID keyboard on macOS — no drivers needed.  
Website shortcuts use Spotlight (`Cmd+Space`) followed by typed URL and Enter.
