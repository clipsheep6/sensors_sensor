/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "print_sensor_data.h"

#include "sensor_errors.h"

#undef LOG_TAG
#define LOG_TAG "PrintSensorData"

namespace OHOS {
namespace Sensors {
namespace {
enum {
    ONE_DIMENSION = 1,
    TWO_DIMENSION = 2,
    THREE_DIMENSION = 3,
    SEVEN_DIMENSION = 7,
    DEFAULT_DIMENSION = 16
};
constexpr int64_t LOG_INTERVAL = 30000000000;
}

void PrintSensorData::ControlSensorHdiPrint(const SensorData &sensorData)
{
    if (sensorData.sensorTypeId == SENSOR_TYPE_ID_HALL_EXT || sensorData.sensorTypeId == SENSOR_TYPE_ID_PROXIMITY
        || sensorData.sensorTypeId == SENSOR_TYPE_ID_HALL) {
        PrintHdiData(sensorData);
    }
    if ((sensorData.sensorTypeId == SENSOR_TYPE_ID_POSTURE)
        && ((postureLastTs_ == 0) || (sensorData.timestamp - postureLastTs_ >= LOG_INTERVAL))) {
        PrintHdiData(sensorData);
        postureLastTs_ = sensorData.timestamp;
    }
    if ((sensorData.sensorTypeId == SENSOR_TYPE_ID_AMBIENT_LIGHT)
        && ((ambientLightLastTs_ == 0) || (sensorData.timestamp - ambientLightLastTs_ >= LOG_INTERVAL))) {
        PrintHdiData(sensorData);
        ambientLightLastTs_ = sensorData.timestamp;
    }
    if ((sensorData.sensorTypeId == SENSOR_TYPE_ID_MAGNETIC_FIELD)
        && ((magneticFieldLastTs_ == 0) || (sensorData.timestamp - magneticFieldLastTs_ >= LOG_INTERVAL))) {
        PrintHdiData(sensorData);
        magneticFieldLastTs_ = sensorData.timestamp;
    }
}

void PrintSensorData::PrintHdiData(const SensorData &sensorData)
{
    std::string str;
    str += "sensorId: " + std::to_string(sensorData.sensorTypeId) + ", ";
    str += "timestamp: " + std::to_string(sensorData.timestamp) + ", ";
    int32_t dataDim = GetDataDimension(sensorData.sensorTypeId);
    auto data = reinterpret_cast<const float *>(sensorData.data);
    for (int32_t i = 0; i < dataDim; ++i) {
        str.append(std::to_string(*data));
        if (i != dataDim - 1) {
            str.append(", ");
        }
        ++data;
    }
    str.append("\n");
    SEN_HILOGI("HdiSensorData: %{public}s", str.c_str());
}

int32_t PrintSensorData::GetDataDimension(int32_t sensorId)
{
    switch (sensorId) {
        case SENSOR_TYPE_ID_HALL:
        case SENSOR_TYPE_ID_PROXIMITY:
            return ONE_DIMENSION;
        case SENSOR_TYPE_ID_HALL_EXT:
            return TWO_DIMENSION;
        case SENSOR_TYPE_ID_POSTURE:
            return SEVEN_DIMENSION;
        case SENSOR_TYPE_ID_AMBIENT_LIGHT:
        case SENSOR_TYPE_ID_MAGNETIC_FIELD:
            return THREE_DIMENSION;
        default:
            SEN_HILOGW("Unknown sensorId:%{public}d, size:%{public}d", sensorId, DEFAULT_DIMENSION);
            return DEFAULT_DIMENSION;
    }
}

void PrintSensorData::ControlSensorClientPrint(const SensorEvent &event)
{
    if (event.sensorTypeId == SENSOR_TYPE_ID_HALL_EXT || event.sensorTypeId == SENSOR_TYPE_ID_PROXIMITY
        || event.sensorTypeId == SENSOR_TYPE_ID_HALL) {
        PrintClientData(event);
    }
    if ((event.sensorTypeId == SENSOR_TYPE_ID_POSTURE)
        && ((clientPostureLastTs_ == 0) || (event.timestamp - clientPostureLastTs_ >= LOG_INTERVAL))) {
        PrintClientData(event);
        clientPostureLastTs_ = event.timestamp;
    }
    if ((event.sensorTypeId == SENSOR_TYPE_ID_AMBIENT_LIGHT)
        && ((clientAmbientLightLastTs_ == 0) || (event.timestamp - clientAmbientLightLastTs_ >= LOG_INTERVAL))) {
        PrintClientData(event);
        clientAmbientLightLastTs_ = event.timestamp;
    }
    if ((event.sensorTypeId == SENSOR_TYPE_ID_MAGNETIC_FIELD)
        && ((clientMagneticFieldLastTs_ == 0) || (event.timestamp - clientMagneticFieldLastTs_ >= LOG_INTERVAL))) {
        PrintClientData(event);
        clientMagneticFieldLastTs_ = event.timestamp;
    }
}

void PrintSensorData::PrintClientData(const SensorEvent &event)
{
    std::string str;
    str += "sensorId: " + std::to_string(event.sensorTypeId) + ", ";
    str += "timestamp: " + std::to_string(event.timestamp) + ", ";
    int32_t dataDim = GetDataDimension(event.sensorTypeId);
    auto data = reinterpret_cast<const float *>(event.data);
    for (int32_t i = 0; i < dataDim; ++i) {
        str.append(std::to_string(*data));
        if (i != dataDim - 1) {
            str.append(", ");
        }
        ++data;
    }
    str.append("\n");
    SEN_HILOGI("ClientSensorData: %{public}s", str.c_str());
}

void PrintSensorData::ControlSensorJsPrint(SensorEvent *event)
{
    if (event->sensorTypeId == SENSOR_TYPE_ID_HALL_EXT || event->sensorTypeId == SENSOR_TYPE_ID_PROXIMITY
        || event->sensorTypeId == SENSOR_TYPE_ID_HALL) {
        PrintJsData(event);
    }
    if ((event->sensorTypeId == SENSOR_TYPE_ID_POSTURE)
        && ((jsPostureLastTs_ == 0) || (event->timestamp - jsPostureLastTs_ >= LOG_INTERVAL))) {
        PrintJsData(event);
        jsPostureLastTs_ = event->timestamp;
    }
    if ((event->sensorTypeId == SENSOR_TYPE_ID_AMBIENT_LIGHT)
        && ((jsAmbientLightLastTs_ == 0) || (event->timestamp - jsAmbientLightLastTs_ >= LOG_INTERVAL))) {
        PrintJsData(event);
        jsAmbientLightLastTs_ = event->timestamp;
    }
    if ((event->sensorTypeId == SENSOR_TYPE_ID_MAGNETIC_FIELD)
        && ((jsMagneticFieldLastTs_ == 0) || (event->timestamp - jsMagneticFieldLastTs_ >= LOG_INTERVAL))) {
        PrintJsData(event);
        jsMagneticFieldLastTs_ = event->timestamp;
    }
}

void PrintSensorData::PrintJsData(SensorEvent *event)
{
    std::string str;
    str += "sensorId: " + std::to_string(event->sensorTypeId) + ", ";
    str += "timestamp: " + std::to_string(event->timestamp) + ", ";
    int32_t dataDim = GetDataDimension(event->sensorTypeId);
    auto data = reinterpret_cast<const float *>(event->data);
    for (int32_t i = 0; i < dataDim; ++i) {
        str.append(std::to_string(*data));
        if (i != dataDim - 1) {
            str.append(", ");
        }
        ++data;
    }
    str.append("\n");
    SEN_HILOGI("ClientSensorData: %{public}s", str.c_str());
}
} // namespace Sensors
} // namespace OHOS
