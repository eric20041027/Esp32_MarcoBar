#pragma once
#include <Arduino.h>

#define PIN_VRX     34
#define PIN_VRY     35
#define PIN_SW      32

#define DEAD_ZONE   200
#define CENTER      2048
#define DEBOUNCE_MS 200
#define LONG_PRESS_MS 1000

enum Direction { DIR_NONE, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

static unsigned long _lastMoveTime   = 0;
static unsigned long _btnPressStart  = 0;
static bool          _btnWasPressed  = false;
static bool          _longFired      = false;

inline void joystickInit() {
  pinMode(PIN_SW, INPUT_PULLUP);
}

inline Direction joystickRead() {
  unsigned long now = millis();
  if (now - _lastMoveTime < DEBOUNCE_MS) return DIR_NONE;

  int x = analogRead(PIN_VRX) - CENTER;
  int y = analogRead(PIN_VRY) - CENTER;

  Direction dir = DIR_NONE;
  if      (x >  DEAD_ZONE) dir = DIR_RIGHT;
  else if (x < -DEAD_ZONE) dir = DIR_LEFT;
  else if (y >  DEAD_ZONE) dir = DIR_DOWN;
  else if (y < -DEAD_ZONE) dir = DIR_UP;

  if (dir != DIR_NONE) _lastMoveTime = now;
  return dir;
}

// Returns true once per press (after release)
inline bool buttonPressed() {
  bool held = (digitalRead(PIN_SW) == LOW);
  unsigned long now = millis();

  if (held && !_btnWasPressed) {
    _btnPressStart = now;
    _btnWasPressed = true;
    _longFired     = false;
  }

  if (!held && _btnWasPressed) {
    _btnWasPressed = false;
    long dur = now - _btnPressStart;
    if (dur < LONG_PRESS_MS) return true;
  }
  return false;
}

// Returns true once when held >= LONG_PRESS_MS
inline bool buttonLongPressed() {
  if (!_btnWasPressed || _longFired) return false;
  if (millis() - _btnPressStart >= LONG_PRESS_MS) {
    _longFired = true;
    return true;
  }
  return false;
}
