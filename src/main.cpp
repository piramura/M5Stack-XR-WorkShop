#include <M5GFX.h>
#include <M5Unified.h>

m5::touch_detail_t touchDetail;
static int32_t w;
static int32_t h;
static bool lastPressed = false;

LGFX_Button button;

static void drawStatus(const char* text) {
  const int statusAreaHeight = 72;
  const int textY = 10;
  M5.Display.fillRect(0, 0, w, statusAreaHeight, WHITE);
  M5.Display.drawString(text, w / 2, textY, &fonts::FreeMonoBold12pt7b);
}

void setup() {
  M5.begin();
  M5.Display.setRotation(1);

  w = M5.Display.width();
  h = M5.Display.height();

  M5.Display.fillScreen(WHITE);
  M5.Display.setTextDatum(top_center);
  drawStatus("Touch Ready");

  const int buttonSize = 200;
  button.initButton(&M5.Display, w / 2, h / 2, buttonSize, buttonSize, TFT_BLUE, TFT_YELLOW, TFT_BLACK, "BTN", 4, 4);
  button.drawButton();
}

void loop() {
  M5.update();
  touchDetail = M5.Touch.getDetail();

  const bool pressed = touchDetail.isPressed() && button.contains(touchDetail.x, touchDetail.y);
  if (pressed != lastPressed) {
    lastPressed = pressed;
    drawStatus(pressed ? "Button Pressed" : "Button Released");
  }
}