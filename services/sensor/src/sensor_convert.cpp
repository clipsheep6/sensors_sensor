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

#include "sensor_convert.h"

#include "report_data_callback.h"
#include "securec.h"
#include "sensor_mapping.h"
#include "sensors_errors.h"
#include "sensors_log_domain.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SensorsLogDomain::SENSOR_ADAPTER, "SensorDataCallback" };
constexpr uint32_t INVALID_SENSOR_ID = -1;

enum {
    RET_OK = 0,
    COPY_FAIL = -1,
    DATA_TYPE_UNSUPPORT = -2,
};

enum {
    X_VALUE = 0,
    Y_VALUE = 1,
    Z_VALUE = 2,
    X_BIAS_VALUE = 3,
    Y_BIAS_VALUE = 4,
    Z_BIAS_VALUE = 5,
    W_VALUE = 3,
};

enum {
    LIGHT_DIMENSION = 3,
    POSE_6DOF_DIMENSION = 15,
    DEFAULT_DIMENSION = 16,
};
}  // namespace

static std::map<SensorTypeTag, ConvertFun> g_convertMap {
    { SENSOR_TYPE_ACCELEROMETER, SensorDataCallback::ConvertToMotionCali },
    { SENSOR_TYPE_LINEAR_ACCELERATION, SensorDataCallback::ConvertToMotionCali },
    { SENSOR_TYPE_GRAVITY, SensorDataCallback::ConvertToMotionCali },
    { SENSOR_TYPE_GYROSCOPE, SensorDataCallback::ConvertToMotionCali },
    { SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED, SensorDataCallback::ConvertToMotionUncali },
    { SENSOR_TYPE_GYROSCOPE_UNCALIBRATED, SensorDataCallback::ConvertToMotionUncali },
    { SENSOR_TYPE_SIGNIFICANT_MOTION, SensorDataCallback::ConvertToSignMotion },
    { SensorType::STEP_DETECTOR, SensorDataCallback::ConvertToStepDetector },
    { SensorType::STEP_COUNTER, SensorDataCallback::ConvertToStepCounter },

    { SENSOR_TYPE_AMBIENT_TEMPERATURE, SensorDataCallback::ConvertToEnvironmentOther },
    { SENSOR_TYPE_HUMIDITY, SensorDataCallback::ConvertToEnvironmentOther },
    { SENSOR_TYPE_MAGNETIC_FIELD, SensorDataCallback::ConvertToEnvironmentCali },
    { SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED, SensorDataCallback::ConvertToEnvironmentUnCali },
    { SensorType::PRESSURE, SensorDataCallback::ConvertToPressuePsAls },

    { SensorType::POSE_6DOF, SensorDataCallback::ConvertToOrientationPose },
    { SENSOR_TYPE_DEVICE_ORIENTATION, SensorDataCallback::ConvertToOrientationDevice },
    { SENSOR_TYPE_ORIENTATION, SensorDataCallback::ConvertToOrientation },
    { SENSOR_TYPE_ROTATION_VECTOR, SensorDataCallback::ConvertToOrientationVec },
    { SENSOR_TYPE_GAME_ROTATION_VECTOR, SensorDataCallback::ConvertToOrientationVec },
    { SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR, SensorDataCallback::ConvertToOrientationVec },

    { SENSOR_TYPE_PROXIMITY, SensorDataCallback::ConvertToPressuePsAls },
    { SENSOR_TYPE_AMBIENT_LIGHT, SensorDataCallback::ConvertToPressuePsAls },

    { SensorType::META_DATA, SensorDataCallback::ConvertToMetaData },
    { SensorType::DYNAMIC_SENSOR_META, SensorDataCallback::ConvertToUnsupport },
    { SensorType::ADDITIONAL_INFO, SensorDataCallback::ConvertToUnsupport },

    { SENSOR_TYPE_HEART_RATE, SensorDataCallback::ConvertToHeartRate },
    { SensorType::LOW_LATENCY_OFFBODY_DETECT, SensorDataCallback::ConvertToOffbodyDetect },
};

int32_t SensorDataCallback::ConvertToHeartRate(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.body.data[0] = aEvent.u.heartRate.bpm;
    zEvent.body.data[1] = static_cast<int8_t>(aEvent.u.heartRate.status);
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToOffbodyDetect(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.body.data[0] = aEvent.u.scalar;
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToStepCounter(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.motion.data[0] = aEvent.u.stepCount;
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToStepDetector(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.motion.data[0] = aEvent.u.scalar;
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToMotionCali(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.motion.data[X_VALUE] = aEvent.u.vec3.x;
    zEvent.motion.data[Y_VALUE] = aEvent.u.vec3.y;
    zEvent.motion.data[Z_VALUE] = aEvent.u.vec3.z;
    zEvent.accuracy = static_cast<int32_t>(aEvent.u.vec3.status);
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToMotionUncali(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.motion.data[X_VALUE] = aEvent.u.uncal.x;
    zEvent.motion.data[Y_VALUE] = aEvent.u.uncal.y;
    zEvent.motion.data[Z_VALUE] = aEvent.u.uncal.z;
    zEvent.motion.data[X_BIAS_VALUE] = aEvent.u.uncal.x_bias;
    zEvent.motion.data[Y_BIAS_VALUE] = aEvent.u.uncal.y_bias;
    zEvent.motion.data[Z_BIAS_VALUE] = aEvent.u.uncal.z_bias;
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToSignMotion(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.motion.data[0] = aEvent.u.scalar;
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToEnvironmentCali(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.environment.data[X_VALUE] = aEvent.u.vec3.x;
    zEvent.environment.data[Y_VALUE] = aEvent.u.vec3.y;
    zEvent.environment.data[Z_VALUE] = aEvent.u.vec3.z;
    zEvent.accuracy = static_cast<int32_t>(aEvent.u.vec3.status);
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToEnvironmentUnCali(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.environment.data[X_VALUE] = aEvent.u.uncal.x;
    zEvent.environment.data[Y_VALUE] = aEvent.u.uncal.y;
    zEvent.environment.data[Z_VALUE] = aEvent.u.uncal.z;
    zEvent.environment.data[X_BIAS_VALUE] = aEvent.u.uncal.x_bias;
    zEvent.environment.data[Y_BIAS_VALUE] = aEvent.u.uncal.y_bias;
    zEvent.environment.data[Z_BIAS_VALUE] = aEvent.u.uncal.z_bias;
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToEnvironmentOther(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.environment.data[0] = aEvent.u.scalar;
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToOrientationPose(const SensorEvent &event, struct SensorEvent &zEvent)
{
    for (int32_t i = 0; i < POSE_6DOF_DIMENSION; ++i) {
        zEvent.orientation.data[i] = aEvent.u.pose6DOF[i];
    }
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToOrientationDevice(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.orientation.data[0] = aEvent.u.scalar;
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToOrientation(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.orientation.data[X_VALUE] = aEvent.u.vec3.x;
    zEvent.orientation.data[Y_VALUE] = aEvent.u.vec3.y;
    zEvent.orientation.data[Z_VALUE] = aEvent.u.vec3.z;
    zEvent.accuracy = static_cast<int32_t>(aEvent.u.vec3.status);
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToOrientationVec(const SensorEvent &event, struct SensorEvent &zEvent)
{
    zEvent.orientation.data[X_VALUE] = aEvent.u.vec4.x;
    zEvent.orientation.data[Y_VALUE] = aEvent.u.vec4.y;
    zEvent.orientation.data[Z_VALUE] = aEvent.u.vec4.z;
    zEvent.orientation.data[W_VALUE] = aEvent.u.vec4.w;
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToPressuePsAls(const SensorEvent &event, struct SensorEvent &zEvent)
{
    for (int32_t i = 0; i < LIGHT_DIMENSION; i++) {
        zEvent.light.data[i] = aEvent.u.data[i];
    }
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToMetaData(const SensorEvent &event, struct SensorEvent &zEvent)
{
    SensorMapping sensorMapping;
    zEvent.other.data[0] = static_cast<int32_t>(aEvent.u.meta.what);
    // for Meta data, meta sensorId is stored in the 'sensorId' of zEvent header,
    // real sensorid store in zEvent.other.reserved[0]
    uint32_t realType = sensorDevice_.GetSensorTypeFromHandle(aEvent.sensorHandle);
    zEvent.other.reserved[0] = sensorMapping.GetSensorIdByAType(static_cast<ASensorType>(realType));
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToUnsupport(const SensorEvent &event, struct SensorEvent &zEvent)
{
    return DATA_TYPE_UNSUPPORT;
}

std::mutex SensorDataCallback::dataMutex_;
std::condition_variable SensorDataCallback::dataCondition_;

int32_t SensorDataCallback::ConvertToDefaultEvent(const SensorEvent &event, struct SensorEvent &zEvent)
{
    if (aEvent.sensorType >= SensorType::DEVICE_PRIVATE_BASE) {
        // data is defined with 16 float array
        if (memcpy_s(zEvent.other.data, DEFAULT_DIMENSION * sizeof(float), aEvent.u.data.data(),
            DEFAULT_DIMENSION * sizeof(float)) != EOK) {
            HiLog::Error(LABEL, "%{public}s : memcpy_s is failed", __func__);
            return COPY_FAIL;
        }
    }
    return ERR_OK;
}

int32_t SensorDataCallback::ConvertToSensorEvent(const SensorEvent &event, struct SensorEvent &zEvent)
{
    int32_t sensorTypeId = event.sensorTypeId;
    if (sensorId == INVALID_SENSOR_ID) {
        HiLog::Error(LABEL, "%{public}s failed", __func__);
        return ERR_INVALID_VALUE;
    }

    zEvent = { .version = sensorDevice_.GetHalDeviceVersion(),
               .sensorId = sensorId,
               .flags = 0,
               .timestamp = aEvent.timestamp,
               .accuracy = 0 };

    auto it = g_convertMap.find(aEvent.sensorType);
    if (it != g_convertMap.end()) {
        return it->second(aEvent, zEvent);
    }
    return ConvertToDefaultEvent(aEvent, zEvent);
}
}  // namespace Sensors
}  // namespace OHOS
