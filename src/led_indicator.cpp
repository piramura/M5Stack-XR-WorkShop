#include "led_indicator.h"

#include <FastLED.h>
#include <M5Unified.h>

namespace {
constexpr size_t LED_COUNT = 29;
constexpr uint8_t LED_BRIGHTNESS = 32;

#if defined(WORKSHOP_M5STICKS3)
constexpr uint8_t LED_DATA_PIN = 9;
#elif defined(ARDUINO_M5STACK_CORES3)
constexpr uint8_t LED_DATA_PIN = 2;
#else
constexpr uint8_t LED_DATA_PIN = 32;
#endif

CRGB leds[LED_COUNT];

void showLedColor(const CRGB& color) {
  fill_solid(leds, LED_COUNT, color);
  FastLED.show();
}
}  // namespace

void initializeLedIndicator() {
#if defined(WORKSHOP_M5STICKS3)
  M5.Power.setExtOutput(true);
#endif

  FastLED.addLeds<SK6812, LED_DATA_PIN, GRB>(leds, LED_COUNT);
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 300);
  showLedColor(CRGB::Black);
}

void setLedPressed(bool pressed) {
  showLedColor(pressed ? CRGB::Blue : CRGB::Black);
}
