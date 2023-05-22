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

#include "sensoronremoterequest_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <iostream>

#include "message_parcel.h"
#include "sensor.h"
#include "sensor_service.h"


namespace OHOS {
namespace Sensors {
namespace {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
const std::u16string SENSOR_INTERFACE_TOKEN = u"ISensorService";
}

class SensorOnRemoteRequestFuzzTest : public SensorServiceStub {
public:
    SensorOnRemoteRequestFuzzTest() {}
    virtual ~SensorOnRemoteRequestFuzzTest() = default;
    ErrCode EnableSensor(int32_t sensorId, int64_t samplingPeriodNs,
                                int64_t maxReportDelayNs) {return 0;};
    ErrCode DisableSensor(int32_t sensorId) {return 0;};
    std::vector<Sensor> GetSensorList() {return {};};
    ErrCode TransferDataChannel(const sptr<Sensors::SensorBasicDataChannel> &sensorBasicDataChannel,
                                    const sptr<IRemoteObject> &sensorClient) {return 0;};
    ErrCode DestroySensorChannel(sptr<IRemoteObject> sensorClient) {return 0;};
    ErrCode SuspendSensors(int32_t pid) {return 0;};
    ErrCode ResumeSensors(int32_t pid) {return 0;};
    ErrCode GetActiveInfoList(int32_t pid, std::vector<Sensors::ActiveInfo> &activeInfoList) {return 0;};
    ErrCode CreateSocketChannel(sptr<IRemoteObject> sensorClient, int32_t &clientFd) {return 0;};
    ErrCode DestroySocketChannel(sptr<IRemoteObject> sensorClient) {return 0;};
    ErrCode EnableActiveInfoCB() {return 0;};
    ErrCode DisableActiveInfoCB() {return 0;};
};

uint32_t GetU32Data(const char* ptr)
{
    // convert fuzz input data to an integer
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}

bool OnRemoteRequestFuzzTest(const char* data, size_t size)
{
    uint32_t code = GetU32Data(data);
    MessageParcel datas;
    datas.WriteInterfaceToken(SENSOR_INTERFACE_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    std::shared_ptr<Sensors::SensorServiceStub> sensorServiceStub = std::make_shared<SensorOnRemoteRequestFuzzTest>();
    sensorServiceStub->OnRemoteRequest(code, datas, reply, option);
    return true;
}
}  // namespace Sensors
}  // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
        /* Run your code on data */
    if (data == nullptr) {
        std::cout << "invalid data" << std::endl;
        return 0;
    }

    /* Validate the length of size */
    if (size > OHOS::Sensors::FOO_MAX_LEN || size < OHOS::Sensors::U32_AT_SIZE) {
        return 0;
    }

    char* ch = (char*)malloc(size + 1);
    if (ch == nullptr) {
        std::cout << "malloc failed." << std::endl;
        return 0;
    }

    (void)memset_s(ch, size + 1, 0x00, size + 1);
    if (memcpy_s(ch, size, data, size) != EOK) {
        std::cout << "copy failed." << std::endl;
        free(ch);
        ch = nullptr;
        return 0;
    }
    OHOS::Sensors::OnRemoteRequestFuzzTest(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}
