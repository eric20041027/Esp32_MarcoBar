#pragma once

// ── Menu structure ──────────────────────────────────────────
struct MenuItem {
  const char* label;
};

struct MenuPage {
  const char* title;
  MenuItem items[4];
  uint8_t itemCount;
};

static const MenuPage PAGES[] = {
  { "Media",   { {"Vol +"},  {"Vol -"},  {"Mute"},  {"Play"} },  4 },
  { "Media 2", { {"Prev"},   {"Next"},   {""},       {""} },      2 },
  { "Sites",   { {"YouTube"}, {"GitHub"}, {"ChatGPT"}, {"Claude"} }, 4 },
  { "System",  { {"Lock"},   {"Screenshot"}, {""}, {""} },        2 },
  { "Window",  { {"Mission"}, {"Input"},  {"Desktop"}, {""} },   3 },
  { "Sensor",  { {"Temp/Hum"}, {""}, {""}, {""} },               1 },
};

static const uint8_t PAGE_COUNT = sizeof(PAGES) / sizeof(PAGES[0]);

// ── State ────────────────────────────────────────────────────
static uint8_t _page = 0;
static uint8_t _item = 0;

inline uint8_t currentPage() { return _page; }
inline uint8_t currentItem() { return _item; }

inline void nextPage() {
  _page = (_page + 1) % PAGE_COUNT;
  _item = 0;
}

inline void prevPage() {
  _page = (_page == 0) ? PAGE_COUNT - 1 : _page - 1;
  _item = 0;
}

inline void nextItem() {
  _item = (_item + 1) % PAGES[_page].itemCount;
}

inline void prevItem() {
  uint8_t count = PAGES[_page].itemCount;
  _item = (_item == 0) ? count - 1 : _item - 1;
}

inline void resetToFirstPage() {
  _page = 0;
  _item = 0;
}
