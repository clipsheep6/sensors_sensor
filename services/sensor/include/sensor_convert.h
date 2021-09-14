/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SENSOR_DATA_CALLBACK_H
#define SENSOR_DATA_CALLBACK_H

#include <vendor/huawei/hardware/sensors/1.2/ISensors.h>
#include <condition_variable>
#include <mutex>
#include "sensor_agent_type.h"
#include "refbase.h"
#include "sensor_device.h"

namespace OHOS {
namespace Sensors {
using ConvertFun = int32_t (*)(const SensorEvent &event, struct SensorEvent &zEvent);

class SensorConvert {
public:
    int32_t ConvertToSensorEvent(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToHeartRate(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToOffbodyDetect(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToStepDetector(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToStepCounter(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToMetaData(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToMotionCali(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToMotionUncali(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToSignMotion(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToEnvironmentCali(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToEnvironmentUnCali(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToEnvironmentOther(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToOrientationPose(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToOrientationDevice(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToOrientation(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToOrientationVec(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToPressuePsAls(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToUnsupport(const SensorEvent &event, struct SensorEvent &zEvent);
    static int32_t ConvertToDefaultEvent(const SensorEvent &event, struct SensorEvent &zEvent);

private:
    static SensorDevice &sensorDevice_;
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // SENSOR_DATA_CALLBACK_H
