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

#include "app_sensor.h"
#include "sensors_errors.h"

namespace OHOS {
namespace Sensors {
using namespace OHOS::HiviewDFX;
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, SENSOR_LOG_DOMAIN, "AppSensor" };
}

bool AppSensor::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt32(pid)) {
        SEN_HILOGE("failed, write pid failed");
        return false;
    }
    if (!parcel.WriteInt32(uid)) {
        SEN_HILOGE("failed, write uid failed");
        return false;
    }
    if (!parcel.WriteUint32(tokenId)) {
        SEN_HILOGE("failed, write tokenId failed");
        return false;
    }
    if (!parcel.WriteUint32(sensorId)) {
        SEN_HILOGE("failed, write sensorId failed");
        return false;
    }
    if (!parcel.WriteInt64(samplingPeriodNs)) {
        SEN_HILOGE("failed, write samplingPeriodNs failed");
        return false;
    }
    if (!parcel.WriteInt64(maxReportDelayNs)) {
        SEN_HILOGE("failed, write maxReportDelayNs failed");
        return false;
    }
    return true;
}

std::unique_ptr<AppSensor> AppSensor::Unmarshalling(Parcel &parcel)
{
    auto appSensor = std::make_unique<AppSensor>();
    if (!appSensor->ReadFromParcel(parcel)) {
        SEN_HILOGE("ReadFromParcel is failed");
        return nullptr;
    }
    return appSensor;
}

bool AppSensor::ReadFromParcel(Parcel &parcel)
{
    if ((!parcel.ReadInt32(pid)) || (!parcel.ReadInt32(uid)) || (!parcel.ReadUint32(tokenId)) ||
        (!parcel.ReadUint32(sensorId)) || (!parcel.ReadInt64(samplingPeriodNs)) ||
        (!parcel.ReadInt64(maxReportDelayNs))) {
        return false;
    }
    return true;
}
}  // namespace Sensors
}  // namespace OHOS
