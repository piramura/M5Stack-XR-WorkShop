#include <M5Unified.h>

#include "imu_sensor.h"
#include "led_indicator.h"
#include "osc_transport.h"
#include "touch_input.h"

void setup() {
  M5.begin();
  Serial.begin(115200);

  initializeTouchInput();
  initializeLedIndicator();
  initializeImuSensor();
  initializeOscTransport();
}

void loop() {
  M5.update();

  const TouchEvent touch = updateTouchInput();
  if (touch.changed) {
    setLedPressed(touch.pressed);
    sendTouchEvent(touch.pressed, touch.x, touch.y);
  }

  ImuQuaternion orientation;
  if (updateImuSensor(orientation)) {
    sendImuData(orientation.w, orientation.x, orientation.y, orientation.z);
  }
}
