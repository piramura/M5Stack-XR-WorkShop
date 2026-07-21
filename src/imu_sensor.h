#pragma once

struct ImuQuaternion {
  float w;
  float x;
  float y;
  float z;
};

void initializeImuSensor();
bool updateImuSensor(ImuQuaternion& orientation);
