#include <FastLED.h>
#include <M5GFX.h>
#include <M5Unified.h>

// 次の行のコメントを外すと、タッチイベントをUnityへOSC送信します。
#define ENABLE_OSC

#ifdef ENABLE_OSC
#include <OSCMessage.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "wifi_config.h"
#endif

static int32_t screenWidth;
static int32_t screenHeight;
static bool lastPressed = false;

LGFX_Button button;

static constexpr size_t LED_COUNT = 29;
static constexpr uint8_t LED_BRIGHTNESS = 32;

#if defined(ARDUINO_M5STACK_CORES3)
static constexpr uint8_t LED_DATA_PIN = 2;
#else
static constexpr uint8_t LED_DATA_PIN = 32;
#endif

static CRGB leds[LED_COUNT];

static void showLedColor(const CRGB& color) {
  fill_solid(leds, LED_COUNT, color);
  FastLED.show();
}

#ifdef ENABLE_OSC
static WiFiUDP oscUdp;

static void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  Serial.printf("WiFi connecting to %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(250);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("WiFi connected: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("WiFi connect timeout");
  }
}

static void sendTouchEvent(bool pressed, int32_t x, int32_t y) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("OSC skipped: WiFi not connected");
    return;
  }

  OSCMessage message(UNITY_TOUCH_OSC_PATH);
  message.add(static_cast<int32_t>(pressed ? 1 : 0));
  message.add(x);
  message.add(y);

  oscUdp.beginPacket(UNITY_OSC_HOST, UNITY_OSC_PORT);
  message.send(oscUdp);
  oscUdp.endPacket();
  Serial.printf("OSC sent: %s pressed=%d x=%d y=%d\n", UNITY_TOUCH_OSC_PATH, pressed ? 1 : 0, x, y);
}
#endif

static void drawStatus(const char* text) {
  constexpr int statusAreaHeight = 72;
  constexpr int textY = 10;
  M5.Display.fillRect(0, 0, screenWidth, statusAreaHeight, WHITE);
  M5.Display.drawString(text, screenWidth / 2, textY, &fonts::FreeMonoBold12pt7b);
}

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.Display.setRotation(1);

  screenWidth = M5.Display.width();
  screenHeight = M5.Display.height();

  M5.Display.fillScreen(WHITE);
  M5.Display.setTextDatum(top_center);
  drawStatus("Touch Ready");

  constexpr int buttonSize = 200;
  button.initButton(&M5.Display, screenWidth / 2, screenHeight / 2,
                    buttonSize, buttonSize, TFT_BLUE, TFT_YELLOW,
                    TFT_BLACK, "BTN", 4, 4);
  button.drawButton();

  FastLED.addLeds<SK6812, LED_DATA_PIN, GRB>(leds, LED_COUNT);
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 300);
  showLedColor(CRGB::Black);

#ifdef ENABLE_OSC
  connectWiFi();
#endif
}

void loop() {
  M5.update();

  const auto touch = M5.Touch.getDetail();
  const bool pressed = touch.isPressed() && button.contains(touch.x, touch.y);

  if (pressed != lastPressed) {
    lastPressed = pressed;
    drawStatus(pressed ? "Button Pressed" : "Button Released");
    showLedColor(pressed ? CRGB::Blue : CRGB::Black);
    Serial.printf("touch %s x=%d y=%d\n",
                  pressed ? "pressed" : "released", touch.x, touch.y);

#ifdef ENABLE_OSC
    sendTouchEvent(pressed, touch.x, touch.y);
#endif
  }
}
