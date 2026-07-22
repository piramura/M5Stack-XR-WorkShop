#pragma once

// Copy this file to wifi_config.h and edit the values for your network and Unity targets.

static constexpr const char* WIFI_SSID = "YOUR_WIFI_SSID";
static constexpr const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

struct OscDestination {
  const char* name;
  const char* host;
};

static constexpr OscDestination OSC_DESTINATIONS[] = {
    {"PC A", "192.168.0.10"},
    {"PC B", "192.168.0.11"},
    {"PC C", "192.168.0.12"},
    // {"PC D", "192.168.0.13"}, // この送信先を使う場合はコメントを外します。
};

static constexpr uint16_t UNITY_OSC_PORT = 9000;

static constexpr const char* UNITY_TOUCH_OSC_PATH = "/m5/touch";
