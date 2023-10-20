/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <sensor_capi.h>

#include <sensor_agent.h>
#include "sensor_errors.h"

using OHOS::HiviewDFX::HiLog;
using OHOS::HiviewDFX::HiLogLabel;
static const HiLogLabel LABEL = {LOG_CORE, OHOS::Sensors::SENSOR_LOG_DOMAIN, "SensorCapiAPI"};

Sensor_Result OH_Sensor_GetAllSensors(Sensor_SensorInfo **sensorInfo, int32_t *count)
{
    Sensor_Result ret = static_cast<Sensor_Result>(GetAllSensors((SensorInfo **)sensorInfo, count));
    if (ret != SENSOR_SUCCESS) {
        SEN_HILOGE("Get sensor list failed, ret is %{public}d", ret);
    }
    return ret;
}

bool CheckSensorTypeId(int32_t sensorTypeId)
{
    int32_t count = -1;
    Sensor_SensorInfo *sensorInfo = nullptr;
    int32_t ret = OH_Sensor_GetAllSensors(&sensorInfo, &count);
    if (ret != 0) {
        return false;
    }
    for (int32_t i = 0; i < count; i++)
    {
        if ((sensorInfo + i)->sensorTypeId == sensorTypeId) {
            return true;
        }
    }
    return false;
}

Sensor_Result OH_Sensor_SubscribeSensor(Sensor_SensorTypeId sensorTypeId, Sensor_SubscribeAttribute attribute,
    const Sensor_SensorUser *user)
{
    if (!CheckSensorTypeId(sensorTypeId) || attribute.samplingInterval < 0 || attribute.reportInterval < 0) {
        SEN_HILOGE("Sensor attribute value is invalid");
        return SENSOR_PARAMETER_ERROR;
    }
    Sensor_Result ret = static_cast<Sensor_Result>(SubscribeSensor(sensorTypeId, (SensorUser *)user));
    if (ret != SENSOR_SUCCESS) {
        SEN_HILOGE("SubscribeSensor failed, ret is %{public}d", ret);
    }
    return ret;
}

Sensor_Result OH_Sensor_UnsubscribeSensor(Sensor_SensorTypeId sensorTypeId, const Sensor_SensorUser *user)
{
    if (!CheckSensorTypeId(sensorTypeId)) {
        SEN_HILOGE("sensorTypeId is invalid");
        return SENSOR_PARAMETER_ERROR;
    }
    Sensor_Result ret = static_cast<Sensor_Result>(UnsubscribeSensor(sensorTypeId, (SensorUser *)user));
    if (ret != SENSOR_SUCCESS) {
        SEN_HILOGE("UnsubscribeSensor failed, ret is %{public}d", ret);
    }
    return ret;
}
