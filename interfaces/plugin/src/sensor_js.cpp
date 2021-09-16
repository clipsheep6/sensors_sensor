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
#include "sensor_js.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <pthread.h>
#include <string>
#include <thread>
#include <unistd.h>

#include "hilog/log.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "sensor_agent.h"
#include "sensor_napi_utils.h"

#include "refbase.h"
#include "securec.h"

using namespace OHOS::HiviewDFX;
static constexpr HiLogLabel LABEL = {LOG_CORE, 0xD002708, "SensorJsAPI"};

static std::map<int32_t, struct AsyncCallbackInfo*> g_onceCallbackInfos;
static std::map<int32_t, struct AsyncCallbackInfo*> g_onCallbackInfos;

static void DataCallbackImpl(SensorEvent *event)
{
    HiLog::Info(LABEL, "%{public}s in", __func__);
    if (event == nullptr) {
        HiLog::Error(LABEL, "%{public}s event is null!", __func__);
        return;
    }
    int32_t sensorTypeId = event->sensorTypeId;
    float *data = (float *)(event->data);
    for (auto &onCallbackInfo: g_onCallbackInfos) {
        if ((int32_t)onCallbackInfo.first == sensorTypeId) {
            onCallbackInfo.second->sensorTypeId = sensorTypeId;
            onCallbackInfo.second->sensorDataLength = event->dataLen;
            if (memcpy_s(onCallbackInfo.second->sensorData, sizeof(data), data, sizeof(data)) != EOK) {
                HiLog::Error(LABEL, "%{public}s copy data failed", __func__);
                return;
            }
            onCallbackInfo.second->status = 1;
            EmitAsyncCallbackWork((struct AsyncCallbackInfo *)(onCallbackInfo.second));
        }
    }
    if (g_onceCallbackInfos.find(sensorTypeId) == g_onceCallbackInfos.end()) {
        HiLog::Debug(LABEL, "%{public}s no subscribe to the sensor data once", __func__);
        return;
    }
    struct AsyncCallbackInfo *onceCallbackInfo = g_onceCallbackInfos[sensorTypeId];
    onceCallbackInfo->sensorTypeId = sensorTypeId;
    onceCallbackInfo->sensorDataLength = event->dataLen;
    if (memcpy_s(onceCallbackInfo->sensorData, sizeof(data), data, sizeof(data)) != EOK) {
        HiLog::Error(LABEL, "%{public}s copy data failed", __func__);
        return;
    }
    onceCallbackInfo->status = 2;
    EmitAsyncCallbackWork((struct AsyncCallbackInfo *)(onceCallbackInfo));
    if (g_onCallbackInfos.find(sensorTypeId) == g_onCallbackInfos.end()) {
        HiLog::Debug(LABEL, "%{public}s no subscription to change sensor data, need to cancel registration", __func__);
        UnsubscribeSensor(sensorTypeId);
    }
    g_onceCallbackInfos.erase(sensorTypeId);
    HiLog::Info(LABEL, "%{public}s end", __func__);
}

static const SensorUser user = {
    .callback = DataCallbackImpl
};

static int32_t UnsubscribeSensor(int32_t sensorTypeId)
{
    HiLog::Info(LABEL, "%{public}s in", __func__);
    int32_t ret = UnsubscribeSensor(sensorTypeId, &user);
    if (ret < 0) {
        HiLog::Error(LABEL, "%{public}s  UnsubscribeSensor failed", __func__);
        return ret;
    }
    ret = DeactivateSensor(sensorTypeId, &user);
    if (ret < 0) {
        HiLog::Error(LABEL, "%{public}s  DeactivateSensor failed", __func__);
        return ret;
    }
    HiLog::Info(LABEL, "%{public}s left", __func__);
    return 0;
}

static int32_t SubscribeSensor(int32_t sensorTypeId, int64_t interval, RecordSensorCallback callback)
{
    HiLog::Info(LABEL, "%{public}s in, sensorTypeId: %{public}d", __func__, sensorTypeId);
    int32_t ret = SubscribeSensor(sensorTypeId, &user);
    if (ret < 0) {
        HiLog::Error(LABEL, "%{public}s subscribeSensor failed", __func__);
        return ret;
    }
    ret = SetBatch(sensorTypeId, &user, interval, 0);
    if (ret < 0) {
        HiLog::Error(LABEL, "%{public}s set batch failed", __func__);
        return ret;
    }
    ret = ActivateSensor(sensorTypeId, &user);
    if (ret < 0) {
        HiLog::Error(LABEL, "%{public}s activateSensor failed", __func__);
        return ret;
    }
    return 0;
}

static napi_value Once(napi_env env, napi_callback_info info)
{
    HiLog::Info(LABEL, "%{public}s in", __func__);
    size_t argc = 2;
    napi_value args[2];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, NULL));

    if (argc != 2) {
        HiLog::Error(LABEL, "%{public}s Invalid input, number of argument should be two", __func__);
        return nullptr;
    }
    if (!IsMatchType(args[0], napi_number, env)) {
        HiLog::Error(LABEL, "%{public}s argument should be number type", __func__);
        return nullptr;
    }
    int32_t sensorTypeId = GetCppInt32(args[0], env);
    if (!IsMatchType(args[1], napi_function, env)) {
        HiLog::Error(LABEL, "%{public}s argument should be function type", __func__);
        return nullptr;
    }
    AsyncCallbackInfo *asyncCallbackInfo = new AsyncCallbackInfo {
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
    };
    napi_create_reference(env, args[1], 1, &asyncCallbackInfo->callback[0]);
    g_onceCallbackInfos[sensorTypeId] = asyncCallbackInfo;
    if (g_onCallbackInfos.find(sensorTypeId) == g_onCallbackInfos.end()) {
        HiLog::Debug(LABEL, "%{public}s no subscription to change sensor data, registration is required", __func__);
        int32_t ret = SubscribeSensor(sensorTypeId, 200000000, DataCallbackImpl);
        if (ret < 0) {
            HiLog::Error(LABEL, "%{public}s subscribe Sensor failed", __func__);
            asyncCallbackInfo->status = -1;
            EmitAsyncCallbackWork(asyncCallbackInfo);
            g_onceCallbackInfos.erase(sensorTypeId);
            return nullptr;
        }
    }
    HiLog::Info(LABEL, "%{public}s left", __func__);
    return nullptr;
}

static napi_value On(napi_env env, napi_callback_info info)
{
    HiLog::Info(LABEL, "%{public}s in", __func__);
    size_t argc = 3;
    napi_value args[3];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, NULL));
    if (argc <= 1 || argc > 3) {
        HiLog::Error(LABEL, "%{public}s the number of input parameters does not match", __func__);
        return nullptr;
    }
    if (!IsMatchType(args[0], napi_number, env)) {
        HiLog::Error(LABEL, "%{public}s the first parameter should be napi_number type", __func__);
        return nullptr;
    }
    int32_t sensorTypeId = GetCppInt32(args[0], env);
    if (!IsMatchType(args[1], napi_function, env)) {
        HiLog::Error(LABEL, "%{public}s the second parameter should be napi_function type!", __func__);
        return nullptr;
    }
    int64_t interval = 200000000;
    if (argc == 3) {
        HiLog::Info(LABEL, "%{public}s argc = 3!", __func__);
        napi_value value = NapiGetNamedProperty(args[2], "interval", env);
        if (!IsMatchType(value, napi_number, env)) {
            HiLog::Error(LABEL, "%{public}s argument should be napi_number type!", __func__);
            return nullptr;
        }
        interval = GetCppInt64(value, env);
    }
    AsyncCallbackInfo *asyncCallbackInfo = new AsyncCallbackInfo {
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
    };
    napi_create_reference(env, args[1], 1, &asyncCallbackInfo->callback[0]);
    g_onCallbackInfos[sensorTypeId] = asyncCallbackInfo;
    int32_t ret = SubscribeSensor(sensorTypeId, interval, DataCallbackImpl);
    if (ret < 0) {
        HiLog::Error(LABEL, "%{public}s subscribeSensor  failed", __func__);
        asyncCallbackInfo->status = -1;
        EmitAsyncCallbackWork(asyncCallbackInfo);
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        g_onCallbackInfos.erase(sensorTypeId);
        return nullptr;
    }
    HiLog::Info(LABEL, "%{public}s out", __func__);
    return nullptr;
}

static napi_value Off(napi_env env, napi_callback_info info)
{
    HiLog::Info(LABEL, "%{public}s in", __func__);
    size_t argc = 2;
    napi_value args[2];
    napi_value thisVar;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisVar, NULL));

    if (argc < 1) {
        HiLog::Error(LABEL, "%{public}s Invalid input.", __func__);
        return nullptr;
    }
    if (!IsMatchType(args[0], napi_number, env)) {
        HiLog::Error(LABEL, "%{public}s argument should be number type!", __func__);
        return nullptr;
    }
    int32_t sensorTypeId = GetCppInt32(args[0], env);
    if (!IsMatchType(args[1], napi_function, env)) {
        HiLog::Error(LABEL, "%{public}s argument should be function type!", __func__);
        return nullptr;
    }
    AsyncCallbackInfo *asyncCallbackInfo = new AsyncCallbackInfo {
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
    };
    napi_create_reference(env, args[1], 1, &asyncCallbackInfo->callback[0]);
    int32_t ret = UnsubscribeSensor(sensorTypeId);
    if (ret < 0) {
        asyncCallbackInfo->status = -1;
        HiLog::Error(LABEL, "%{public}s UnsubscribeSensor failed", __func__);
    } else {
        HiLog::Error(LABEL, "%{public}s UnsubscribeSensor success", __func__);
        asyncCallbackInfo->status = 0;
        if (g_onCallbackInfos.find(sensorTypeId) != g_onCallbackInfos.end()) {
            napi_delete_reference(env, g_onCallbackInfos[sensorTypeId]->callback[0]);
            napi_delete_async_work(env, g_onCallbackInfos[sensorTypeId]->asyncWork);
            delete g_onCallbackInfos[sensorTypeId];
            g_onCallbackInfos[sensorTypeId] = nullptr;
            g_onCallbackInfos.erase(sensorTypeId);
        }
    }
    EmitAsyncCallbackWork(asyncCallbackInfo);
    HiLog::Info(LABEL, "%{public}s left", __func__);
    return nullptr;
}

EXTERN_C_START

static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("on", On),
        DECLARE_NAPI_FUNCTION("once", Once),
        DECLARE_NAPI_FUNCTION("off", Off)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));
    return exports;
}
EXTERN_C_END

static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = NULL,
    .nm_register_func = Init,
    .nm_modname = "sensor",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
