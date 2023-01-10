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

#include "sensor_callback_stub.h"

#include "hisysevent.h"
#include "message_parcel.h"

#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, SENSOR_LOG_DOMAIN, "SensorCallbackStub" };
}  // namespace

int32_t SensorCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                            MessageOption &option)
{
    SEN_HILOGD("begin, cmd:%{public}u, flags:%{public}d", code, option.GetFlags());
    std::u16string descriptor = SensorCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        SEN_HILOGE("SensorCallbackStub::OnRemoteRequest failed, descriptor mismatch");
        return OBJECT_NULL;
    }
    int32_t msgCode = static_cast<int32_t>(code);
    if (msgCode == ISensorCallback::SENSOR_CHANGE) {
        int32_t pid;
        int32_t uid;
        uint32_t tokenId;
        uint32_t sensorId;
        int64_t samplingPeriodNs;
        int64_t maxReportDelayNs;
        if ((!data.ReadInt32(pid)) || (!data.ReadInt32(uid)) || (!data.ReadUint32(tokenId)) ||
            (!data.ReadUint32(sensorId)) || (!data.ReadInt64(samplingPeriodNs)) ||
            (!data.ReadInt64(maxReportDelayNs))) {
            SEN_HILOGE("Parcel read failed");
            return ERROR;
        }
        AppSensorInfo appSensorInfo;
        appSensorInfo.appThreadInfo.pid = pid;
        appSensorInfo.appThreadInfo.uid = uid;
        appSensorInfo.appThreadInfo.callerToken = static_cast<AccessTokenID>(tokenId);
        appSensorInfo.sensorId = sensorId;
        appSensorInfo.samplingPeriodNs = samplingPeriodNs;
        appSensorInfo.maxReportDelayNs = maxReportDelayNs;
        OnSensorChanged(appSensorInfo);
    } else {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NO_ERROR;
}
}  // namespace Sensors
}  // namespace OHOS
