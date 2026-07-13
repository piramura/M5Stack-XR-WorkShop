#include <M5GFX.h>
#include <M5Unified.h>
#include <OSCMessage.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "wifi_config.h"

static WiFiUDP oscUdp;
static bool lastPressed = false;

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

void setup() {
  M5.begin();
  Serial.begin(115200);
  connectWiFi();
}

void loop() {
  M5.update();

  const auto touch = M5.Touch.getDetail();
  const bool pressed = touch.isPressed();

  if (pressed != lastPressed) {
    lastPressed = pressed;
    sendTouchEvent(pressed, touch.x, touch.y);
  }
}