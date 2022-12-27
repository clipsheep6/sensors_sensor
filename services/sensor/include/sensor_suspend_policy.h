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

#ifndef SENSOR_SUSPEND_POLICY_H
#define SENSOR_SUSPEND_POLICY_H

#include <mutex>
#include <unordered_map>

#include "singleton.h"
#include "client_info.h"
#include "sensor_manager.h"
#include "flush_info_record.h"
#include "sensor_hdi_connection.h"
#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
class SensorSuspendPolicy {
    DECLARE_DELAYED_SINGLETON(SensorSuspendPolicy);
public:
    DISALLOW_COPY_AND_MOVE(SensorSuspendPolicy);
    ErrCode DoSuspend(int32_t pid);
    ErrCode DoResume(int32_t pid);

private:
    bool CheckFreezingSensor(uint32_t sensorId);
    ErrCode DisableSensor(uint32_t sensorId, int32_t pid);
    ErrCode EnableSensor(uint32_t sensorId, int32_t pid, int64_t samplingPeriodNs, int64_t maxReportDelayNs);
    ErrCode RestoreSensorInfo(uint32_t sensorId, int32_t pid, int64_t samplingPeriodNs, int64_t maxReportDelayNs);
    std::mutex suspendMutex_;
    SensorHdiConnection &sensorHdiConnection_ = SensorHdiConnection::GetInstance();
    ClientInfo &clientInfo_ = ClientInfo::GetInstance();
    SensorManager &sensorManager_ = SensorManager::GetInstance();
    FlushInfoRecord &flushInfo_ = FlushInfoRecord::GetInstance();
    std::unordered_map<int32_t, std::unordered_map<uint32_t, SensorBasicInfo>> pidSensorInfoMap_;
};
#define SuspendPolicy DelayedSingleton<SensorSuspendPolicy>::GetInstance()
}  // namespace Sensors
}  // namespace OHOS
#endif // SENSOR_SUSPEND_POLICY_H
