#include "touch_input.h"

#include <M5GFX.h>
#include <M5Unified.h>

namespace {
int32_t screenWidth;
bool lastPressed = false;
LGFX_Button button;

void drawStatus(const char* text) {
  constexpr int statusAreaHeight = 72;
  constexpr int textY = 10;
  M5.Display.fillRect(0, 0, screenWidth, statusAreaHeight, WHITE);
  M5.Display.drawString(text, screenWidth / 2, textY,
                        &fonts::FreeMonoBold12pt7b);
}
}  // namespace

void initializeTouchInput() {
  M5.Display.setRotation(1);
  screenWidth = M5.Display.width();
  const int32_t screenHeight = M5.Display.height();

  M5.Display.fillScreen(WHITE);
  M5.Display.setTextDatum(top_center);
  drawStatus("Touch Ready");

  constexpr int buttonSize = 200;
  button.initButton(&M5.Display, screenWidth / 2, screenHeight / 2,
                    buttonSize, buttonSize, TFT_BLUE, TFT_YELLOW, TFT_BLACK,
                    "BTN", 4, 4);
  button.drawButton();
}

TouchEvent updateTouchInput() {
  const auto touch = M5.Touch.getDetail();
  const bool pressed = touch.isPressed() && button.contains(touch.x, touch.y);
  const bool changed = pressed != lastPressed;

  if (changed) {
    lastPressed = pressed;
    drawStatus(pressed ? "Button Pressed" : "Button Released");
    Serial.printf("touch %s x=%d y=%d\n",
                  pressed ? "pressed" : "released", touch.x, touch.y);
  }

  return {changed, pressed, touch.x, touch.y};
}
