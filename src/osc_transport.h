#pragma once

#include <cstdint>

// 次の行をコメントアウトすると、OSC送信とWi-Fi接続を無効化します。
// #define ENABLE_OSC

void initializeOscTransport();
void sendTouchEvent(bool pressed, int32_t x, int32_t y);
void sendImuData(float w, float x, float y, float z);
