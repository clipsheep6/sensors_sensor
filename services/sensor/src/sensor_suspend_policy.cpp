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

#include "sensor.h"
#include "sensors_errors.h"
#include "sensor_suspend_policy.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SENSOR_LOG_DOMAIN, "SensorSuspendPolicy" };
constexpr uint32_t INVALID_SENSOR_ID = -1;
constexpr int64_t MAX_EVENT_COUNT = 1000;
constexpr uint32_t STEP_COUNTER_ID = 524544;
constexpr uint32_t STEP_DETECTOR_ID = 590080;
}  // namespace

SensorSuspendPolicy::SensorSuspendPolicy() {}

SensorSuspendPolicy::~SensorSuspendPolicy() {}

bool SensorSuspendPolicy::CheckFreezingSensor(uint32_t sensorId)
{
    return ((sensorId == STEP_COUNTER_ID) || (sensorId == STEP_DETECTOR_ID));
}

ErrCode SensorSuspendPolicy::DoSuspend(int32_t pid)
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> suspendLock(suspendMutex_);
    auto pidSensorInfoIt = pidSensorInfoMap_.find(pid);
    if (pidSensorInfoIt != pidSensorInfoMap_.end()) {
        SEN_HILOGE("pid sensors already suspend, not need suspend again, pid: %{public}d", pid);
        return ERROR;
    }
    std::vector<uint32_t> sensorIdList = clientInfo_.GetSensorIdByPid(pid);
    if (sensorIdList.empty()) {
        SEN_HILOGE("pid sensorId list is empty, pid: %{public}d", pid);
        return ERROR;
    }
    bool suspendAllSensors = true;
    std::unordered_map<uint32_t, SensorBasicInfo> SensorInfoMap;
    for (auto &sensorId : sensorIdList) {
        if (CheckFreezingSensor(sensorId)) {
            continue;
        }
        auto sensorInfo = clientInfo_.GetCurPidSensorInfo(sensorId, pid);
        SensorInfoMap.emplace(std::make_pair(sensorId, sensorInfo));
        auto ret = DisableSensor(sensorId, pid);
        if (ret != ERR_OK) {
            SEN_HILOGE("Disable Sensor is failed. sensorId: %{public}u, pid: %{public}d, ret: %{public}d",
                       sensorId, pid, ret);
            suspendAllSensors = false;
        }
    }
    pidSensorInfoMap_.emplace(std::make_pair(pid, SensorInfoMap));
    if (!suspendAllSensors) {
        SEN_HILOGE("some sensor disable failed");
        return DISABLE_SENSOR_ERR;
    }
    return ERR_OK;
}

ErrCode SensorSuspendPolicy::DisableSensor(uint32_t sensorId, int32_t pid)
{
    CALL_LOG_ENTER;
    if (sensorId == INVALID_SENSOR_ID) {
        SEN_HILOGE("sensorId is invalid");
        return ERR_NO_INIT;
    }
    if (sensorManager_.IsOtherClientUsingSensor(sensorId, pid)) {
        SEN_HILOGW("other client is using this sensor now, cannot disable");
        return ERR_OK;
    }
    if (sensorHdiConnection_.DisableSensor(sensorId) != ERR_OK) {
        SEN_HILOGE("DisableSensor is failed");
        return DISABLE_SENSOR_ERR;
    }
    return sensorManager_.AfterDisableSensor(sensorId);
}

ErrCode SensorSuspendPolicy::DoResume(int32_t pid)
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> suspendLock(suspendMutex_);
    auto pidSensorInfoIt = pidSensorInfoMap_.find(pid);
    if (pidSensorInfoIt == pidSensorInfoMap_.end()) {
        SEN_HILOGE("pid not have suspend sensors, pid: %{public}d", pid);
        return ERROR;
    }
    bool resumeAllSensors = true;
    std::unordered_map<uint32_t, SensorBasicInfo> SensorInfoMap = pidSensorInfoIt->second;
    for (auto &it : SensorInfoMap) {
        uint32_t sensorId = it.first;
        int64_t samplePeriod = it.second.GetSamplingPeriodNs();
        int64_t maxReportDelay = it.second.GetMaxReportDelayNs();
        auto ret = EnableSensor(sensorId, pid, samplePeriod, maxReportDelay);
        if (ret != ERR_OK) {
            SEN_HILOGE("Enable Sensor is failed. sensorId: %{public}u, pid: %{public}d, ret: %{public}d",
                       sensorId, pid, ret);
            resumeAllSensors = false;
        }
    }
    if (!resumeAllSensors) {
        SEN_HILOGE("some sensor enable failed");
        return ENABLE_SENSOR_ERR;
    }
    pidSensorInfoMap_.erase(pidSensorInfoIt);
    return ERR_OK;
}

ErrCode SensorSuspendPolicy::EnableSensor(uint32_t sensorId, int32_t pid, int64_t samplingPeriodNs,
                                          int64_t maxReportDelayNs)
{
    CALL_LOG_ENTER;
    if ((sensorId == INVALID_SENSOR_ID) || (samplingPeriodNs == 0) ||
        ((samplingPeriodNs != 0L) && (maxReportDelayNs / samplingPeriodNs > MAX_EVENT_COUNT))) {
        SEN_HILOGE("sensorId is 0 or maxReportDelayNs exceed the maximum value");
        return ERR_NO_INIT;
    }
    if (clientInfo_.GetSensorState(sensorId)) {
        SEN_HILOGW("sensor has been enabled already, sensorId: %{public}d", sensorId);
        auto ret = RestoreSensorInfo(sensorId, pid, samplingPeriodNs, maxReportDelayNs);
        if (ret != ERR_OK) {
            SEN_HILOGE("RestoreSensorInfo is failed, ret: %{public}d", ret);
            return ret;
        }
        uint32_t flag = sensorManager_.GetSensorFlag(sensorId);
        ret = flushInfo_.FlushProcess(sensorId, flag, pid, true);
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

ErrCode SensorSuspendPolicy::RestoreSensorInfo(uint32_t sensorId, int32_t pid, int64_t samplingPeriodNs,
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
}  // namespace Sensors
}  // namespace OHOS
