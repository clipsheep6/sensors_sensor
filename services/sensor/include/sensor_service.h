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

#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

#include <mutex>
#include <thread>
#include <unordered_map>

#include "nocopyable.h"
#include "system_ability.h"

#include "client_info.h"
#include "death_recipient_template.h"
#include "sensor_data_event.h"
#include "sensor_hdi_connection.h"
#include "sensor_manager.h"
#include "sensor_service_stub.h"
#include "sensor_suspend_policy.h"
#include "stream_server.h"

namespace OHOS {
namespace Sensors {
enum class SensorServiceState {
    STATE_STOPPED,
    STATE_RUNNING,
};

class SensorService : public SystemAbility, public StreamServer, public SensorServiceStub {
    DECLARE_SYSTEM_ABILITY(SensorService)

public:
    explicit SensorService(int32_t systemAbilityId, bool runOnCreate = false);
    virtual ~SensorService() = default;
    void OnDump() override;
    void OnStart() override;
    void OnStop() override;
    int Dump(int fd, const std::vector<std::u16string> &args) override;
    ErrCode EnableSensor(int32_t sensorId, int64_t samplingPeriodNs, int64_t maxReportDelayNs) override;
    ErrCode DisableSensor(int32_t sensorId) override;
    std::vector<Sensor> GetSensorList() override;
    ErrCode TransferDataChannel(const sptr<SensorBasicDataChannel> &sensorBasicDataChannel,
                                const sptr<IRemoteObject> &sensorClient) override;
    ErrCode DestroySensorChannel(sptr<IRemoteObject> sensorClient) override;
    void ProcessDeathObserver(const wptr<IRemoteObject> &object);
    ErrCode SuspendSensors(int32_t pid) override;
    ErrCode ResumeSensors(int32_t pid) override;
    ErrCode GetSubscribeInfoList(int32_t pid, std::vector<SubscribeInfo> &subscribeInfoList) override;
    ErrCode CreateSocketChannel(int32_t &clientFd, const sptr<IRemoteObject> &sensorClient) override;
    ErrCode DestroySocketChannel(const sptr<IRemoteObject> &sensorClient) override;
    ErrCode EnableClientInfoCallback() override;
    ErrCode DisableClientInfoCallback() override;

private:
    DISALLOW_COPY_AND_MOVE(SensorService);
    void RegisterClientDeathRecipient(sptr<IRemoteObject> sensorClient, int32_t pid);
    void UnregisterClientDeathRecipient(sptr<IRemoteObject> sensorClient);
    bool InitInterface();
    bool InitDataCallback();
    bool InitSensorList();
    bool InitSensorPolicy();
    void ReportOnChangeData(int32_t sensorId);
    void ReportSensorSysEvent(int32_t sensorId, bool enable, int32_t pid);
    ErrCode DisableSensor(int32_t sensorId, int32_t pid);
    SensorServiceState state_;
    std::mutex serviceLock_;
    std::mutex sensorsMutex_;
    std::mutex sensorMapMutex_;
    std::vector<Sensor> sensors_;
    std::unordered_map<int32_t, Sensor> sensorMap_;
    SensorHdiConnection &sensorHdiConnection_ = SensorHdiConnection::GetInstance();
    ClientInfo &clientInfo_ = ClientInfo::GetInstance();
    SensorManager &sensorManager_ = SensorManager::GetInstance();
    FlushInfoRecord &flushInfo_ = FlushInfoRecord::GetInstance();
    SensorSuspendPolicy &suspendPolicy_ = SensorSuspendPolicy::GetInstance();
    sptr<SensorDataProcesser> sensorDataProcesser_ = nullptr;
    sptr<ReportDataCallback> reportDataCallback_ = nullptr;
    std::mutex uidLock_;
    // death recipient of sensor client
    sptr<IRemoteObject::DeathRecipient> clientDeathObserver_ = nullptr;
    ErrCode SaveSubscriber(int32_t sensorId, int64_t samplingPeriodNs, int64_t maxReportDelayNs);

    std::atomic_bool IsReportClientInfo_ = false;
    void ReportClientInfo(int32_t sensorId, bool isActive, int32_t pid);
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // SENSOR_SERVICE_H
