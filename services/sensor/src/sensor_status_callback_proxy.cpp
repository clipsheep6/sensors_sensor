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

#include "sensor_status_callback_proxy.h"

#include "message_parcel.h"
#include "hisysevent.h"

#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, SENSOR_LOG_DOMAIN, "SensorStatusCallbackProxy" };
}  // namespace


void SensorStatusCallbackProxy::OnSensorChanged(const AppSensorInfo &appSensorInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(SensorStatusCallbackProxy::GetDescriptor())) {
        SEN_HILOGE("Write descriptor failed");
        return;
    }
    if (!data.WriteInt32(appSensorInfo.pid)) {
        SEN_HILOGE("Write pid failed");
        return;
    }
    if (!data.WriteInt32(appSensorInfo.sensorId)) {
        SEN_HILOGE("Write sensorId failed");
        return;
    }
    if (!data.WriteBool(appSensorInfo.isActive)) {
        SEN_HILOGE("Write isActive failed");
        return;
    }
    if (!data.WriteInt64(appSensorInfo.samplingPeriodNs)) {
        SEN_HILOGE("Write samplingPeriodNs failed");
        return;
    }
    if (!data.WriteInt64(appSensorInfo.maxReportDelayNs)) {
        SEN_HILOGE("Write maxReportDelayNs failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPV(remote);
    int32_t ret = remote->SendRequest(ISensorStatusCallback::SENSOR_CHANGE, data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SENSOR, "SENSOR_CALLBACK_IPC_EXCEPTION",
            HiviewDFX::HiSysEvent::EventType::FAULT, "PKG_NAME", "OnSensorChanged", "ERROR_CODE", ret);
        SEN_HILOGE("Failed, ret:%{public}d", ret);
        return;
    }
}
}  // namespace Sensors
}  // namespace OHOS
