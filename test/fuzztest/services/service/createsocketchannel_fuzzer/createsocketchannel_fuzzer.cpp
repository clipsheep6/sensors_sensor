/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "createsocketchannel_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "accesstoken_kit.h"
#include "message_parcel.h"
#include "nativetoken_kit.h"
#include "securec.h"
#include "token_setproc.h"

#include "sensor.h"
#include "sensor_service.h"

namespace OHOS {
namespace Sensors {
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;
namespace {
constexpr size_t U32_AT_SIZE = 4;
auto g_service = SensorDelayedSpSingleton<SensorService>::GetInstance();
static sptr<IRemoteObject> g_remote = new (std::nothrow) IPCObjectStub();
} // namespace

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    errno_t ret = memcpy_s(&object, objectSize, data, objectSize);
    if (ret != EOK) {
        return 0;
    }
    return objectSize;
}

void SetUpTestCase()
{
    const char **perms = new (std::nothrow) const char *[2];
    if (perms == nullptr) {
        return;
    }
    perms[0] = "ohos.permission.ACCELEROMETER";
    perms[1] = "ohos.permission.MANAGE_SENSOR";
    TokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 2,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "CreateDataChannelStubFuzzTest",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}

bool CreateSocketChannelFuzzTest(const uint8_t *data, size_t size)
{
    SetUpTestCase();
    if (g_remote == nullptr || g_service == nullptr) {
        return false;
    }
    int32_t clientFd = 0;
    GetObject<int32_t>(clientFd, data, size);
    g_service->CreateSocketChannel(g_remote, clientFd);
    g_service->OnStart();
    SensorPlugInfo info;
    info.deviceName = "";
    GetObject<int32_t>(info.deviceSensorInfo.deviceId, data, size);
    GetObject<int32_t>(info.deviceSensorInfo.sensorType, data, size);
    GetObject<int32_t>(info.deviceSensorInfo.sensorId, data, size);
    GetObject<int32_t>(info.deviceSensorInfo.location, data, size);
    GetObject<int32_t>(info.status, data, size);
    GetObject<int32_t>(info.reserved, data, size);
    g_service->ReportPlugEventCallback(info);
    g_service->OnStop();
    g_service->ProcessDeathObserver(g_remote);
    return true;
}
}  // namespace Sensors
}  // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    /* Validate the length of size */
    if (size < OHOS::Sensors::U32_AT_SIZE) {
        return 0;
    }

    OHOS::Sensors::CreateSocketChannelFuzzTest(data, size);
    return 0;
}
