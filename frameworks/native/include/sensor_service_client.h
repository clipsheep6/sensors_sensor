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

#ifndef SENSOR_SERVICE_CLIENT_H
#define SENSOR_SERVICE_CLIENT_H

#include <set>

#include "iservice_registry.h"
#include "singleton.h"

#include "sensor_basic_info.h"
#include "sensor_client_stub.h"
#include "sensor_data_channel.h"
#include "sensor_service_proxy.h"
#include "stream_socket.h"

namespace OHOS {
namespace Sensors {
class SensorServiceClient : public StreamSocket, public Singleton<SensorServiceClient> {
public:
    ~SensorServiceClient() override;
    std::vector<Sensor> GetSensorList();
    std::vector<Sensor> GetSensorListByDevice(int32_t deviceId);
    int32_t GetSensorListByDevice(int32_t deviceId, std::vector<Sensor> &singleDevSensors);
    int32_t EnableSensor(const SensorDescription &sensorDesc, int64_t samplingPeriod, int64_t maxReportDelay);
    int32_t DisableSensor(const SensorDescription &sensorDesc);
    int32_t TransferDataChannel(sptr<SensorDataChannel> sensorDataChannel);
    int32_t DestroyDataChannel();
    void ProcessDeathObserver(const wptr<IRemoteObject> &object);
    bool IsValid(const SensorDescription &sensorDesc);
    int32_t SuspendSensors(int32_t pid);
    int32_t ResumeSensors(int32_t pid);
    int32_t GetActiveInfoList(int32_t pid, std::vector<ActiveInfo> &activeInfoList);
    int32_t Register(SensorActiveInfoCB callback, sptr<SensorDataChannel> sensorDataChannel);
    int32_t Unregister(SensorActiveInfoCB callback);
    int32_t ResetSensors();
    void ReceiveMessage(const char *buf, size_t size);
    void Disconnect();
    void HandleNetPacke(NetPacket &pkt);
    void SetDeviceStatus(uint32_t deviceStatus);
    int32_t CreateClientRemoteObject();
    int32_t TransferClientRemoteObject();
    int32_t DestroyClientRemoteObject();
    bool EraseCacheSensorList(const SensorPlugData &info);
    int32_t GetLocalDeviceId(int32_t &deviceId);

private:
    int32_t InitServiceClient();
    void UpdateSensorInfoMap(const SensorDescription &sensorDesc, int64_t samplingPeriod, int64_t maxReportDelay);
    void DeleteSensorInfoItem(const SensorDescription &sensorDesc);
    int32_t CreateSocketClientFd(int32_t &clientFd);
    int32_t CreateSocketChannel();
    void ReenableSensor();
    void WriteHiSysIPCEvent(ISensorServiceIpcCode code, int32_t ret);
    void WriteHiSysIPCEventSplit(ISensorServiceIpcCode code, int32_t ret);
    std::mutex clientMutex_;
    sptr<IRemoteObject::DeathRecipient> serviceDeathObserver_ = nullptr;
    sptr<ISensorService> sensorServer_ = nullptr;
    std::vector<Sensor> sensorList_;
    std::mutex channelMutex_;
    sptr<SensorDataChannel> dataChannel_ = nullptr;
    sptr<SensorClientStub> sensorClientStub_ = nullptr;
    std::mutex mapMutex_;
    std::map<SensorDescription, SensorBasicInfo> sensorInfoMap_;
    std::atomic_bool isConnected_ = false;
    CircleStreamBuffer circBuf_;
    std::mutex activeInfoCBMutex_;
    std::set<SensorActiveInfoCB> activeInfoCBSet_;
};
} // namespace Sensors
} // namespace OHOS
#endif // SENSOR_SERVICE_CLIENT_H