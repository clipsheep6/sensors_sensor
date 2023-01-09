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

#include "sensor_callback_proxy.h"

#include "message_parcel.h"
#include "hisysevent.h"

#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, SENSOR_LOG_DOMAIN, "SensorCallbackProxy" };
}  // namespace


void SensorCallbackProxy::OnSensorChanged(const AppSensorInfo &appSensorInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(SensorCallbackProxy::GetDescriptor())) {
        SEN_HILOGE("write descriptor failed");
        return;
    }
    if (!data.WriteInt32(appSensorInfo.appThreadInfo.pid)) {
        SEN_HILOGE("write pid failed");
        return;
    }
    if (!data.WriteInt32(appSensorInfo.appThreadInfo.uid)) {
        SEN_HILOGE("write uid failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(appSensorInfo.appThreadInfo.callerToken))) {
        SEN_HILOGE("write tokenId failed");
        return;
    }
    if (!data.WriteUint32(appSensorInfo.sensorId)) {
        SEN_HILOGE("write sensorId failed");
        return;
    }
    if (!data.WriteInt64(appSensorInfo.samplingPeriodNs)) {
        SEN_HILOGE("write samplingPeriodNs failed");
        return;
    }
    if (!data.WriteInt64(appSensorInfo.maxReportDelayNs)) {
        SEN_HILOGE("write maxReportDelayNs failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    CHKPV(remote);
    int32_t ret = remote->SendRequest(ISensorCallback::SENSOR_CHANGE, data, reply, option);
    if (ret != NO_ERROR) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SENSOR, "SENSOR_CALLBACK_IPC_EXCEPTION",
            HiviewDFX::HiSysEvent::EventType::FAULT, "PKG_NAME", "OnSensorChanged", "ERROR_CODE", ret);
        SEN_HILOGE("failed, ret:%{public}d", ret);
        return;
    }
}
}  // namespace Sensors
}  // namespace OHOS
