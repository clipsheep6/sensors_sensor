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

#include "sensor_status_callback_stub.h"

#include "hisysevent.h"
#include "message_parcel.h"

#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, SENSOR_LOG_DOMAIN, "SensorStatusCallbackStub" };
}  // namespace

int32_t SensorStatusCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                  MessageOption &option)
{
    SEN_HILOGD("Begin, cmd:%{public}u, flags:%{public}d", code, option.GetFlags());
    std::u16string descriptor = SensorStatusCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        SEN_HILOGE("SensorStatusCallbackStub::OnRemoteRequest failed, descriptor mismatch");
        return OBJECT_NULL;
    }
    if (code != ISensorStatusCallback::SENSOR_CHANGE) {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    int32_t pid;
    int32_t sensorId;
    bool isActive;
    int64_t samplingPeriodNs;
    int64_t maxReportDelayNs;
    if (!(data.ReadInt32(pid) && data.ReadInt32(sensorId) && data.ReadBool(isActive) &&
        data.ReadInt64(samplingPeriodNs) && data.ReadInt64(maxReportDelayNs))) {
        SEN_HILOGE("Parcel read failed");
        return ERROR;
    }
    AppSensorInfo appSensorInfo;
    appSensorInfo.pid = pid;
    appSensorInfo.sensorId = sensorId;
    appSensorInfo.isActive = isActive;
    appSensorInfo.samplingPeriodNs = samplingPeriodNs;
    appSensorInfo.maxReportDelayNs = maxReportDelayNs;
    OnSensorChanged(appSensorInfo);
    return NO_ERROR;
}
}  // namespace Sensors
}  // namespace OHOS
