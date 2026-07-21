#pragma once

#include <cstdint>

struct TouchEvent {
  bool changed;
  bool pressed;
  int32_t x;
  int32_t y;
};

void initializeTouchInput();
TouchEvent updateTouchInput();
