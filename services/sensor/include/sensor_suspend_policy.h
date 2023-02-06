/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <set>
#include <unordered_map>
#include <vector>

#include "app_sensor.h"
#include "client_info.h"
#include "i_sensor_status_callback.h"
#include "sensor_agent_type.h"
#include "sensor_hdi_connection.h"
#include "sensor_manager.h"
#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
class SensorSuspendPolicy : public Singleton<SensorSuspendPolicy>{
public:
    ErrCode DoSuspend(int32_t pid);
    ErrCode DoResume(int32_t pid);
    ErrCode GetAppSensorList(int32_t pid, std::vector<AppSensor> &appSensorList);
    ErrCode AddCallback(sptr<ISensorStatusCallback> callback);
    void ExecuteCallbackAsync(int32_t pid, int32_t sensorId, bool isActive,
                              int64_t samplingPeriodNs, int64_t maxReportDelayNs);
    bool InitDeathRecipient();
    void ProcessDeathObserver(wptr<IRemoteObject> remote);

private:
    struct ClassComp {
        bool operator()(const sptr<ISensorStatusCallback> &callback1,
                        const sptr<ISensorStatusCallback> &callback2) const
        {
            return callback1->AsObject() < callback2->AsObject();
        }
    };
    bool CheckFreezingSensor(int32_t sensorId);
    bool DisableSensorList(std::unordered_map<int32_t, SensorBasicInfo> &SensorInfoMap,
                           std::vector<int32_t> &sensorIdList, int32_t pid);
    ErrCode DisableSensor(std::unordered_map<int32_t, SensorBasicInfo> &SensorInfoMap,
                          int32_t sensorId, int32_t pid);
    ErrCode EnableSensor(int32_t sensorId, int32_t pid, int64_t samplingPeriodNs, int64_t maxReportDelayNs);
    ErrCode RestoreSensorInfo(int32_t sensorId, int32_t pid, int64_t samplingPeriodNs, int64_t maxReportDelayNs);
    SensorHdiConnection &sensorHdiConnection_ = SensorHdiConnection::GetInstance();
    ClientInfo &clientInfo_ = ClientInfo::GetInstance();
    SensorManager &sensorManager_ = SensorManager::GetInstance();
    std::mutex pidSensorInfoMutex_;
    std::unordered_map<int32_t, std::unordered_map<int32_t, SensorBasicInfo>> pidSensorInfoMap_;
    std::mutex callbackMutex_;
    std::set<const sptr<ISensorStatusCallback>, ClassComp> callbackSet_;
    sptr<IRemoteObject::DeathRecipient> sensorStatusCBDeathRecipient_ { nullptr };
};
}  // namespace Sensors
}  // namespace OHOS
#endif // SENSOR_SUSPEND_POLICY_H
