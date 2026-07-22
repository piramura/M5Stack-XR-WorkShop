#pragma once

#include <cstddef>
#include <cstdint>

// 次の行をコメントアウトすると、OSC送信とWi-Fi接続を無効化します。
// #define ENABLE_OSC

void initializeOscTransport();
bool isOscEnabled();
size_t getOscDestinationCount();
const char* getOscDestinationName(size_t index);
void selectOscDestination(size_t index);
void clearOscDestinationSelection();
void sendTouchEvent(bool pressed, int32_t x, int32_t y);
void sendImuData(float w, float x, float y, float z);
