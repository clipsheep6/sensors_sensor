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

#include "sensor_capi.h"

#include "sensor_agent.h"
#include "i_sensor_service.h"
#include "sensor.h"
#include "sensor_service_client.h"
#include "sensor_errors.h"

using OHOS::HiviewDFX::HiLog;
using OHOS::HiviewDFX::HiLogLabel;
static const HiLogLabel LABEL = {LOG_CORE, OHOS::Sensors::SENSOR_LOG_DOMAIN, "SensorCapiAPI"};

Sensor_Result OH_Sensor_GetAllSensors(Sensor_SensorInfo **sensorMessage, int32_t *count)
{
    SensorInfo **sensorInfo = reinterpret_cast<SensorInfo **>(const_cast<Sensor_SensorInfo **>(sensorMessage));
    if (count == nullptr) {
        SEN_HILOGD("Get count fail");
    }
    Sensor_Result ret = static_cast<Sensor_Result>(GetAllSensors(sensorInfo, count));
    if (count == nullptr) {
        SEN_HILOGD("Get count fail");
    }
    if (ret != SENSOR_SUCCESS) {
        SEN_HILOGE("Get sensor list failed, ret is %{public}d", ret);
    }
    return ret;
}

bool CheckSensorSubscribeId(Sensor_SubscribeId subscribeId)
{
    int32_t count = -1;
    Sensor_SensorInfo *sensorInfo = nullptr;
    int32_t ret = OH_Sensor_GetAllSensors(&sensorInfo, &count);
    SEN_HILOGD("Get sensor sensorInfo  %{public}s, %{public}d, %{public}d", sensorInfo->sensorName,
                sensorInfo->sensorTypeId, sensorInfo->sensorId);
    SEN_HILOGD("Get sensor sensorInfo%{public}d", count);
    if (ret != 0) {
        return false;
    }
    for (int32_t i = 0; i < count; i++)
    {
        if ((sensorInfo + i)->sensorTypeId == subscribeId.sensorTypeId) {
            SEN_HILOGD("Get sensor sensorTypeId:%{public}d", subscribeId.sensorTypeId);
            return true;
        }
    }
    return false;
}

Sensor_Result OH_Sensor_SubscribeSensor(Sensor_SubscribeId subscribeId, Sensor_SubscribeAttribute attribute,
    const Sensor_SubscribeUser *cb)
{
    if (!CheckSensorSubscribeId(subscribeId) || attribute.samplingInterval < 0 || attribute.reportInterval < 0) {
        SEN_HILOGE("Sensor attribute value is invalid");
        return SENSOR_PARAMETER_ERROR;
    }
    SEN_HILOGD("SubscribeSensor in");
    SensorUser *user = reinterpret_cast<SensorUser *>(const_cast<Sensor_SubscribeUser *>(cb));
    Sensor_Result ret = static_cast<Sensor_Result>(SubscribeSensor(subscribeId.sensorTypeId, user));
    if (ret != SENSOR_SUCCESS) {
        SEN_HILOGE("SubscribeSensor failed, ret is %{public}d", ret);
    }
    ret = static_cast<Sensor_Result>(SetBatch(subscribeId.sensorTypeId, user,
                                     attribute.samplingInterval, attribute.reportInterval));
    if (ret != SENSOR_SUCCESS) {
        SEN_HILOGE("SetBatch failed, ret is %{public}d", ret);
    }
    ret = static_cast<Sensor_Result>(ActivateSensor(subscribeId.sensorTypeId, user));
    if (ret != SENSOR_SUCCESS) {
        SEN_HILOGE("ActivateSensor failed, ret is %{public}d", ret);
    }
    return ret;
}

Sensor_Result OH_Sensor_UnsubscribeSensor(Sensor_SubscribeId subscribeId, const Sensor_SubscribeUser *cb)
{
    if (!CheckSensorSubscribeId(subscribeId)) {
        SEN_HILOGE("subscribeId is invalid");
        return SENSOR_PARAMETER_ERROR;
    }
    SensorUser *user = reinterpret_cast<SensorUser*>(const_cast<Sensor_SubscribeUser *>(cb));
    Sensor_Result ret = static_cast<Sensor_Result>(DeactivateSensor(subscribeId.sensorTypeId, user));
    if (ret != SENSOR_SUCCESS) {
        SEN_HILOGE("DeactivateSensor failed, ret is %{public}d", ret);
    }
    ret = static_cast<Sensor_Result>(UnsubscribeSensor(subscribeId.sensorTypeId, user));
    if (ret != SENSOR_SUCCESS) {
        SEN_HILOGE("UnsubscribeSensor failed, ret is %{public}d", ret);
    }
    return ret;
}
