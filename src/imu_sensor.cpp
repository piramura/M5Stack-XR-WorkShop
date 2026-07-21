#include "imu_sensor.h"

#include <Adafruit_AHRS.h>
#include <M5Unified.h>

namespace {
constexpr float DEFAULT_SAMPLE_FREQUENCY_HZ = 100.0f;
constexpr float MICROS_PER_SECOND = 1000000.0f;

Adafruit_Madgwick filter;
uint32_t lastSampleUsec = 0;
}  // namespace

void initializeImuSensor() {
  filter.begin(DEFAULT_SAMPLE_FREQUENCY_HZ);
}

bool updateImuSensor(ImuQuaternion& orientation) {
  const auto updated = M5.Imu.update();
  if ((updated & m5::IMU_Class::sensor_mask_accel) == 0 ||
      (updated & m5::IMU_Class::sensor_mask_gyro) == 0) {
    return false;
  }

  const auto& data = M5.Imu.getImuData();
  float deltaSeconds = 1.0f / DEFAULT_SAMPLE_FREQUENCY_HZ;
  if (lastSampleUsec != 0) {
    const uint32_t elapsedUsec = data.usec - lastSampleUsec;
    if (elapsedUsec != 0) {
      deltaSeconds = elapsedUsec / MICROS_PER_SECOND;
    }
  }
  lastSampleUsec = data.usec;

  filter.updateIMU(data.gyro.x, data.gyro.y, data.gyro.z,
                   data.accel.x, data.accel.y, data.accel.z, deltaSeconds);
  filter.getQuaternion(&orientation.w, &orientation.x,
                       &orientation.y, &orientation.z);
  return true;
}
