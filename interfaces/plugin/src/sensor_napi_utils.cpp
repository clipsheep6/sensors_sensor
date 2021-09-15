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

#include "sensor_napi_utils.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "hilog/log.h"

using namespace OHOS::HiviewDFX;
static constexpr HiLogLabel LABEL = {LOG_CORE, 0xD002708, "SensorJsAPI"};
bool IsMatchType(napi_value value, napi_valuetype type, napi_env env)
{
    napi_valuetype paramType;
    napi_typeof(env, value, &paramType);
    if (paramType != type) {
        HiLog::Error(LABEL, "%{public}s  failed!", __func__);
        return false;
    }
    return true;
}

napi_value GetNapiInt32(int32_t number, napi_env env)
{
    napi_value value;
    napi_create_int32(env, number, &value);
    return value;
}

napi_value NapiGetNamedProperty(napi_value jsonObject, std::string name, napi_env env)
{
    napi_value value;
    napi_get_named_property(env, jsonObject, name.c_str(), &value);
    return value;
}

int32_t GetCppInt32(napi_value value, napi_env env)
{
    int32_t number;
    napi_get_value_int32(env, value, &number);
    return number;
}

int64_t GetCppInt64(napi_value value, napi_env env)
{
    int64_t number;
    napi_get_value_int64(env, value, &number);
    return number;
}

bool GetCppBool(napi_value value, napi_env env)
{
    bool number;
    napi_get_value_bool(env, value, &number);
    return number;
}

napi_value GetUndefined(napi_env env)
{
    napi_value value;
    napi_get_undefined(env, &value);
    return value;
}

std::map<int32_t, std::vector<std::string>> g_sensorAttributeList = {
    { 0, { "x" } },
    { SENSOR_TYPE_ID_ACCELEROMETER, { "x", "y", "z" } },
    { SENSOR_TYPE_ID_GYROSCOPE, { "x", "y", "z" } },
    { SENSOR_TYPE_ID_AMBIENT_LIGHT, { "intensity" } },
    { SENSOR_TYPE_ID_MAGNETIC_FIELD, { "x", "y", "z" } },
    { SENSOR_TYPE_ID_BAROMETER, { "pressure" } },
    { SENSOR_TYPE_ID_HALL, { "status" } },
    { SENSOR_TYPE_ID_PROXIMITY, { "distance" } },
    { SENSOR_TYPE_ID_HUMIDITY, { "humidity" } },
    { SENSOR_TYPE_ID_ORIENTATION, { "x", "y", "z" } },
    { SENSOR_TYPE_ID_GRAVITY, { "x", "y", "z" } },
    { SENSOR_TYPE_ID_LINEAR_ACCELERATION, { "x", "y", "z" } },
    { SENSOR_TYPE_ID_ROTATION_VECTOR, { "x", "y", "z" } },
    { SENSOR_TYPE_ID_AMBIENT_TEMPERATURE, { "temperature" } },
    { SENSOR_TYPE_ID_MAGNETIC_FIELD_UNCALIBRATED, { "x", "y", "z", "biasX", "biasY", "biasZ" } },
    { SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED, { "x", "y", "z", "biasX", "biasY", "biasZ" } },
    { SENSOR_TYPE_ID_SIGNIFICANT_MOTION, { "scalar" } },
    { SENSOR_TYPE_ID_PEDOMETER_DETECTION, { "scalar" } },
    { SENSOR_TYPE_ID_PEDOMETER, { "step" } },
    { SENSOR_TYPE_ID_HEART_RATE, { "heartRate" } },
    { SENSOR_TYPE_ID_WEAR_DETECTION, { "value" } },
    { SENSOR_TYPE_ID_ACCELEROMETER_UNCALIBRATED, { "x", "y", "z", "biasX", "biasY", "biasZ" } }
};

void EmitAsyncCallbackWork(AsyncCallbackInfo *asyncCallbackInfo)
{
    HiLog::Debug(LABEL, "%{public}s begin", __func__);
    if (asyncCallbackInfo == nullptr) {
        HiLog::Error(LABEL, "%{public}s asyncCallbackInfo is null!", __func__);
        return;
    }
    napi_value resourceName;
    if (napi_create_string_utf8(asyncCallbackInfo->env, "AsyncCallback", NAPI_AUTO_LENGTH, &resourceName) != napi_ok) {
        HiLog::Error(LABEL, "%{public}s create string utf8 failed", __func__);
        return;
    }
    napi_create_async_work(
        asyncCallbackInfo->env, nullptr, resourceName,
        [](napi_env env, void* data) {},
        [](napi_env env, napi_status status, void* data) {
            HiLog::Debug(LABEL, "%{public}s napi_create_async_work in", __func__);
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            napi_value callback;
            napi_get_reference_value(env, asyncCallbackInfo->callback[0], &callback);
            napi_value callResult = nullptr;
            napi_value result[2] = {0};
            if (asyncCallbackInfo->status < 0) {
                HiLog::Debug(LABEL, "%{public}s napi_create_async_work < 0 in", __func__);
                napi_value code = nullptr;
                napi_value message = nullptr;
                napi_create_string_utf8(env, "-1", NAPI_AUTO_LENGTH, &code);
                napi_create_string_utf8(env, "failed", NAPI_AUTO_LENGTH, &message);
                napi_create_error(env, code, message, &result[0]);
                napi_get_undefined(env, &result[1]);
            } else if (asyncCallbackInfo->status == 0) {
                napi_create_object(env, &result[1]);
                napi_value message = nullptr;
                napi_create_int32(env, asyncCallbackInfo->status, &message);
                napi_set_named_property(env, result[1], "code", message);
                napi_get_undefined(env, &result[0]);
            } else {
                int32_t sensorTypeId = asyncCallbackInfo->sensorTypeId;
                if (g_sensorAttributeList.count(sensorTypeId) == 0) {
                    HiLog::Error(LABEL, "%{public}s count of sensorTypeId is zero", __func__);
                    return;
                }
                std::vector<std::string> sensorAttribute = g_sensorAttributeList[sensorTypeId];
                float *sensorData = asyncCallbackInfo->sensorData;
                napi_create_object(env, &result[1]);
                for (size_t i = 0; i < sensorAttribute.size(); i++) {
                    napi_value message = nullptr;
                    double a = *sensorData;
                    napi_create_double(env, a, &message);
                    napi_set_named_property(env, result[1], sensorAttribute[i].c_str(), message);
                }
                napi_get_undefined(env, &result[0]);
            }
            napi_call_function(env, nullptr, callback, 2, result, &callResult);
            if (asyncCallbackInfo->status != 1) {
                HiLog::Debug(LABEL, "%{public}s not a continuous callback, need to release asyncCallbackInfo", __func__);
                napi_delete_reference(env, asyncCallbackInfo->callback[0]);
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
                asyncCallbackInfo = nullptr;
            }
            HiLog::Debug(LABEL, "%{public}s napi_create_async_work left", __func__);
            free(result);
        },
        asyncCallbackInfo, &asyncCallbackInfo->asyncWork);
    napi_queue_async_work(asyncCallbackInfo->env, asyncCallbackInfo->asyncWork);
    HiLog::Debug(LABEL, "%{public}s end", __func__);
}
