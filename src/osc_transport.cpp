#include "osc_transport.h"

#include <Arduino.h>

#ifdef ENABLE_OSC
#include <OSCMessage.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "wifi_config.h"

namespace {
constexpr char IMU_OSC_PATH[] = "/m5/imu";
constexpr uint32_t IMU_SEND_INTERVAL_MS = 1000 / 30;

WiFiUDP oscUdp;
uint32_t lastImuSentAt = 0;

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void sendMessage(OSCMessage& message) {
  oscUdp.beginPacket(UNITY_OSC_HOST, UNITY_OSC_PORT);
  message.send(oscUdp);
  oscUdp.endPacket();
}
}  // namespace
#endif

void initializeOscTransport() {
#ifdef ENABLE_OSC
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  Serial.printf("WiFi connecting to %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(250);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("WiFi connected: %s\n",
                  WiFi.localIP().toString().c_str());
  } else {
    Serial.println("WiFi connect timeout");
  }
#endif
}

void sendTouchEvent(bool pressed, int32_t x, int32_t y) {
#ifdef ENABLE_OSC
  if (!isWiFiConnected()) {
    Serial.println("OSC skipped: WiFi not connected");
    return;
  }

  OSCMessage message(UNITY_TOUCH_OSC_PATH);
  message.add(static_cast<int32_t>(pressed ? 1 : 0));
  message.add(x);
  message.add(y);
  sendMessage(message);
  Serial.printf("OSC sent: %s pressed=%d x=%d y=%d\n",
                UNITY_TOUCH_OSC_PATH, pressed ? 1 : 0, x, y);
#else
  (void)pressed;
  (void)x;
  (void)y;
#endif
}

void sendImuData(float w, float x, float y, float z) {
#ifdef ENABLE_OSC
  const uint32_t now = millis();
  if (now - lastImuSentAt < IMU_SEND_INTERVAL_MS) {
    return;
  }
  lastImuSentAt = now;

  if (!isWiFiConnected()) {
    return;
  }

  OSCMessage message(IMU_OSC_PATH);
  message.add(w);
  message.add(x);
  message.add(y);
  message.add(z);
  sendMessage(message);
#else
  (void)w;
  (void)x;
  (void)y;
  (void)z;
#endif
}
