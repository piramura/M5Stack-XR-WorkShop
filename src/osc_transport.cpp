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
constexpr size_t OSC_DESTINATION_COUNT =
    sizeof(OSC_DESTINATIONS) / sizeof(OSC_DESTINATIONS[0]);
static_assert(OSC_DESTINATION_COUNT >= 1,
              "At least one OSC destination is required");
static_assert(OSC_DESTINATION_COUNT <= 3,
              "Up to three OSC destinations are supported");

WiFiUDP oscUdp;
uint32_t lastImuSentAt = 0;
size_t selectedDestinationIndex = 0;
bool destinationSelected = false;

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void sendMessage(OSCMessage& message) {
  oscUdp.beginPacket(OSC_DESTINATIONS[selectedDestinationIndex].host,
                     UNITY_OSC_PORT);
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

bool isOscEnabled() {
#ifdef ENABLE_OSC
  return true;
#else
  return false;
#endif
}

size_t getOscDestinationCount() {
#ifdef ENABLE_OSC
  return OSC_DESTINATION_COUNT;
#else
  return 0;
#endif
}

const char* getOscDestinationName(size_t index) {
#ifdef ENABLE_OSC
  if (index < OSC_DESTINATION_COUNT) {
    return OSC_DESTINATIONS[index].name;
  }
#else
  (void)index;
#endif
  return "";
}

void selectOscDestination(size_t index) {
#ifdef ENABLE_OSC
  if (index >= OSC_DESTINATION_COUNT) {
    return;
  }

  selectedDestinationIndex = index;
  destinationSelected = true;
  Serial.printf("OSC target selected: %s (%s)\n",
                OSC_DESTINATIONS[index].name, OSC_DESTINATIONS[index].host);
#else
  (void)index;
#endif
}

void clearOscDestinationSelection() {
#ifdef ENABLE_OSC
  destinationSelected = false;
#endif
}

void sendTouchEvent(bool pressed, int32_t x, int32_t y) {
#ifdef ENABLE_OSC
  if (!isWiFiConnected()) {
    Serial.println("OSC skipped: WiFi not connected");
    return;
  }
  if (!destinationSelected) {
    Serial.println("OSC skipped: target not selected");
    return;
  }

  OSCMessage message(UNITY_TOUCH_OSC_PATH);
  message.add(static_cast<int32_t>(pressed ? 1 : 0));
  message.add(x);
  message.add(y);
  sendMessage(message);
  Serial.printf("OSC sent: target=%s path=%s pressed=%d x=%d y=%d\n",
                OSC_DESTINATIONS[selectedDestinationIndex].name,
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

  if (!isWiFiConnected() || !destinationSelected) {
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
