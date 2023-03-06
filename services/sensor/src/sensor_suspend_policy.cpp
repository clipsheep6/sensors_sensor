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

#include "sensor_suspend_policy.h"

#include "death_recipient_template.h"
#include "sensor.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SENSOR_LOG_DOMAIN, "SensorSuspendPolicy" };
constexpr int32_t INVALID_SENSOR_ID = -1;
constexpr int64_t MAX_EVENT_COUNT = 1000;
}  // namespace

bool SensorSuspendPolicy::InitDeathRecipient()
{
    if (sensorStatusCBDeathRecipient_ == nullptr) {
        sensorStatusCBDeathRecipient_ = new (std::nothrow) DeathRecipientTemplate(*const_cast<SensorSuspendPolicy *>(this));
        if (sensorStatusCBDeathRecipient_ == nullptr) {
            SEN_HILOGE("Failed to get sensorStatusCBDeathRecipient_");
            return false;
        }
    }
    return true;
}

void SensorSuspendPolicy::ProcessDeathObserver(wptr<IRemoteObject> remote)
{
    sptr<IRemoteObject> client = remote.promote();
    CHKPV(client);
    sptr<ISensorStatusCallback> callback = iface_cast<ISensorStatusCallback>(client);
    std::lock_guard<std::mutex> callbackLock(callbackMutex_);
    size_t ret = callbackSet_.erase(callback);
    if (ret != 0) {
        client->RemoveDeathRecipient(sensorStatusCBDeathRecipient_);
    }
}

bool SensorSuspendPolicy::CheckFreezingSensor(int32_t sensorId)
{
    return ((sensorId == SENSOR_TYPE_ID_PEDOMETER_DETECTION) || (sensorId == SENSOR_TYPE_ID_PEDOMETER));
}

ErrCode SensorSuspendPolicy::DoSuspend(int32_t pid)
{
    CALL_LOG_ENTER;
    std::vector<int32_t> sensorIdList = clientInfo_.GetSensorIdByPid(pid);
    std::lock_guard<std::mutex> pidSensorInfoLock(pidSensorInfoMutex_);
    auto pidSensorInfoIt = pidSensorInfoMap_.find(pid);
    if (pidSensorInfoIt != pidSensorInfoMap_.end()) {
        if (sensorIdList.empty()) {
            SEN_HILOGI("Pid already suspend, all sensors suspend success, not need suspend again, pid: %{public}d", pid);
            return ERR_OK;
        } else {
            SEN_HILOGI("Pid already suspend, some sensors suspend failed, suspend these sensors again, pid: %{public}d", pid);
            std::unordered_map<int32_t, SensorBasicInfo> SensorInfoMap = pidSensorInfoIt->second;
            if (!DisableSensorList(SensorInfoMap, sensorIdList, pid)) {
                SEN_HILOGE("Some sensor disable failed");
                return DISABLE_SENSOR_ERR;
            }
            return ERR_OK;
        }
    }
    if (sensorIdList.empty()) {
        SEN_HILOGE("Pid sensorId list is empty, pid: %{public}d", pid);
        return ERROR;
    }
    std::unordered_map<int32_t, SensorBasicInfo> SensorInfoMap;
    bool suspendAllSensors = DisableSensorList(SensorInfoMap, sensorIdList, pid);
    pidSensorInfoMap_.insert(std::make_pair(pid, SensorInfoMap));
    if (!suspendAllSensors) {
        SEN_HILOGE("Some sensor disable failed");
        return DISABLE_SENSOR_ERR;
    }
    return ERR_OK;
}

bool SensorSuspendPolicy::DisableSensorList(std::unordered_map<int32_t, SensorBasicInfo> &SensorInfoMap,
                                            std::vector<int32_t> &sensorIdList, int32_t pid)
{
    CALL_LOG_ENTER;
    bool suspendAllSensors = true;
    for (auto &sensorId : sensorIdList) {
        if (CheckFreezingSensor(sensorId)) {
            continue;
        }
        auto ret = DisableSensor(SensorInfoMap, sensorId, pid);
        if (ret != ERR_OK) {
            SEN_HILOGE("Disable Sensor is failed. sensorId: %{public}u, pid: %{public}d, ret: %{public}d",
                       sensorId, pid, ret);
            suspendAllSensors = false;
        }
    }
    return suspendAllSensors;
}

ErrCode SensorSuspendPolicy::DisableSensor(std::unordered_map<int32_t, SensorBasicInfo> &SensorInfoMap,
                                           int32_t sensorId, int32_t pid)
{
    CALL_LOG_ENTER;
    if (sensorId == INVALID_SENSOR_ID) {
        SEN_HILOGE("SensorId is invalid");
        return ERR_NO_INIT;
    }
    auto sensorInfo = clientInfo_.GetCurPidSensorInfo(sensorId, pid);
    if (sensorManager_.IsOtherClientUsingSensor(sensorId, pid)) {
        SEN_HILOGW("Other client is using this sensor now, cannot disable");
        SensorInfoMap.insert(std::make_pair(sensorId, sensorInfo));
        return ERR_OK;
    }
    if (sensorHdiConnection_.DisableSensor(sensorId) != ERR_OK) {
        SEN_HILOGE("DisableSensor is failed");
        return DISABLE_SENSOR_ERR;
    }
    SensorInfoMap.insert(std::make_pair(sensorId, sensorInfo));
    return sensorManager_.AfterDisableSensor(sensorId);
}

ErrCode SensorSuspendPolicy::DoResume(int32_t pid)
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> pidSensorInfoLock(pidSensorInfoMutex_);
    auto pidSensorInfoIt = pidSensorInfoMap_.find(pid);
    if (pidSensorInfoIt == pidSensorInfoMap_.end()) {
        SEN_HILOGE("Pid not have suspend sensors, pid: %{public}d", pid);
        return ERROR;
    }
    bool resumeAllSensors = true;
    std::unordered_map<int32_t, SensorBasicInfo> SensorInfoMap = pidSensorInfoIt->second;
    for (auto sensorIt = SensorInfoMap.begin(); sensorIt != SensorInfoMap.end();) {
        int32_t sensorId = sensorIt->first;
        int64_t samplingPeriodNs = sensorIt->second.GetSamplingPeriodNs();
        int64_t maxReportDelayNs = sensorIt->second.GetMaxReportDelayNs();
        auto ret = EnableSensor(sensorId, pid, samplingPeriodNs, maxReportDelayNs);
        if (ret != ERR_OK) {
            SEN_HILOGE("Enable Sensor is failed. sensorId: %{public}u, pid: %{public}d, ret: %{public}d",
                       sensorId, pid, ret);
            resumeAllSensors = false;
            ++sensorIt;
        } else {
            sensorIt = SensorInfoMap.erase(sensorIt);
        }
    }
    if (!resumeAllSensors) {
        SEN_HILOGE("Some sensor enable failed");
        return ENABLE_SENSOR_ERR;
    }
    pidSensorInfoMap_.erase(pidSensorInfoIt);
    return ERR_OK;
}

ErrCode SensorSuspendPolicy::EnableSensor(int32_t sensorId, int32_t pid, int64_t samplingPeriodNs,
                                          int64_t maxReportDelayNs)
{
    CALL_LOG_ENTER;
    if ((sensorId == INVALID_SENSOR_ID) || (samplingPeriodNs == 0) ||
        ((samplingPeriodNs != 0L) && (maxReportDelayNs / samplingPeriodNs > MAX_EVENT_COUNT))) {
        SEN_HILOGE("SensorId is 0 or maxReportDelayNs exceed the maximum value");
        return ERR_NO_INIT;
    }
    if (clientInfo_.GetSensorState(sensorId)) {
        SEN_HILOGW("Sensor has been enabled already, sensorId: %{public}d", sensorId);
        auto ret = RestoreSensorInfo(sensorId, pid, samplingPeriodNs, maxReportDelayNs);
        if (ret != ERR_OK) {
            SEN_HILOGE("RestoreSensorInfo is failed, ret: %{public}d", ret);
            return ret;
        }
        if (ret != ERR_OK) {
            SEN_HILOGE("FlushProcess is failed, ret: %{public}d", ret);
        }
        return ERR_OK;
    }
    auto ret = RestoreSensorInfo(sensorId, pid, samplingPeriodNs, maxReportDelayNs);
    if (ret != ERR_OK) {
        SEN_HILOGE("RestoreSensorInfo is failed, ret: %{public}d", ret);
        return ret;
    }
    ret = sensorHdiConnection_.EnableSensor(sensorId);
    if (ret != ERR_OK) {
        SEN_HILOGE("EnableSensor is failed, ret: %{public}d", ret);
        clientInfo_.RemoveSubscriber(sensorId, pid);
        return ENABLE_SENSOR_ERR;
    }
    return ERR_OK;
}

ErrCode SensorSuspendPolicy::RestoreSensorInfo(int32_t sensorId, int32_t pid, int64_t samplingPeriodNs,
                                               int64_t maxReportDelayNs)
{
    auto ret = sensorManager_.SaveSubscriber(sensorId, pid, samplingPeriodNs, maxReportDelayNs);
    if (ret != ERR_OK) {
        SEN_HILOGE("RestoreSensorInfo is failed, ret: %{public}d", ret);
        return ret;
    }
    sensorManager_.StartDataReportThread();
    if (!sensorManager_.SetBestSensorParams(sensorId, samplingPeriodNs, maxReportDelayNs)) {
        SEN_HILOGE("SetBestSensorParams is failed");
        clientInfo_.RemoveSubscriber(sensorId, pid);
        return ENABLE_SENSOR_ERR;
    }
    return ret;
}

ErrCode SensorSuspendPolicy::GetAppSensorList(int32_t pid, std::vector<AppSensor> &appSensorList)
{
    CALL_LOG_ENTER;
    std::vector<int32_t> sensorIdList = clientInfo_.GetSensorIdByPid(pid);
    for (auto &sensorId : sensorIdList) {
        auto sensorInfo = clientInfo_.GetCurPidSensorInfo(sensorId, pid);
        int64_t samplingPeriodNs = sensorInfo.GetSamplingPeriodNs();
        int64_t maxReportDelayNs = sensorInfo.GetMaxReportDelayNs();
        AppSensor appSensor(pid, sensorId, true, samplingPeriodNs, maxReportDelayNs);
        appSensorList.push_back(appSensor);
    }
    std::lock_guard<std::mutex> pidSensorInfoLock(pidSensorInfoMutex_);
    auto pidSensorInfoIt = pidSensorInfoMap_.find(pid);
    if (pidSensorInfoIt != pidSensorInfoMap_.end()) {
        std::unordered_map<int32_t, SensorBasicInfo> SensorInfoMap = pidSensorInfoIt->second;
        for (auto &sensorInfo : SensorInfoMap) {
            int32_t sensorId = sensorInfo.first;
            int64_t samplingPeriodNs = sensorInfo.second.GetSamplingPeriodNs();
            int64_t maxReportDelayNs = sensorInfo.second.GetMaxReportDelayNs();
            AppSensor appSensor(pid, sensorId, false, samplingPeriodNs, maxReportDelayNs);
            appSensorList.push_back(appSensor);
        }
    }
    return ERR_OK;
}

ErrCode SensorSuspendPolicy::AddCallback(sptr<ISensorStatusCallback> callback)
{
    CALL_LOG_ENTER;
    if (callback == nullptr) {
        SEN_HILOGE("Callback is nullptr");
        return ERROR;
    }
    auto object = callback->AsObject();
    if (object == nullptr) {
        SEN_HILOGE("Callback->AsObject() is nullptr");
        return ERROR;
    }
    std::lock_guard<std::mutex> callbackLock(callbackMutex_);
    auto retPair = callbackSet_.insert(callback);
    if (retPair.second) {
        object->AddDeathRecipient(sensorStatusCBDeathRecipient_);
    }
    return ERR_OK;
}

void SensorSuspendPolicy::ExecuteCallbackAsync(int32_t pid, int32_t sensorId, bool isActive,
                                               int64_t samplingPeriodNs, int64_t maxReportDelayNs)
{
    CALL_LOG_ENTER;
    AppSensorInfo appSensorInfo;
    appSensorInfo.pid = pid;
    appSensorInfo.sensorId = sensorId;
    appSensorInfo.isActive = isActive;
    appSensorInfo.samplingPeriodNs = samplingPeriodNs;
    appSensorInfo.maxReportDelayNs = maxReportDelayNs;
    std::lock_guard<std::mutex> callbackLock(callbackMutex_);
    for (auto callbackIt = callbackSet_.begin(); callbackIt != callbackSet_.end(); ++callbackIt) {
        auto callback = *callbackIt;
        if (callback != nullptr) {
            callback->OnSensorChanged(appSensorInfo);
        }
    }
}
}  // namespace Sensors
}  // namespace OHOS
